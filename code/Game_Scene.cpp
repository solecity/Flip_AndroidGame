/*
 * GAME SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#include <basics/Log>
#include <basics/Canvas>
#include <basics/Director>

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "Game_Scene.hpp"
#include "Pause_Scene.hpp"
#include "Gameover_Scene.hpp"
#include "Pancake.hpp"
#include "Strawberry.hpp"

using namespace basics;
using namespace std;

namespace flip
{

    const char * Game_Scene::background_path        = "game-scene/background.png";
    const char * Game_Scene::prepare_path           = "game-scene/get_ready.png";
    const char * Game_Scene::sprites_atlas_path     = "game-scene/sprite.sprites";
    const char * Game_Scene::font_path              = "game-scene/bubblegum.fnt";


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        canvas_width  = 1280;
        canvas_height =  720;

        srand (unsigned(time(nullptr)));        // Starts the random number generator seed

        initialize ();                          // Other attributes are initialized

        suspended = true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Game_Scene::initialize ()
    {
        state = LOADING;
        //state             = background.get () ? RUNNING : LOADING;

        gameplay          = UNINITIALIZED;

        suspended         = false;

        score_counter     = 0;
        lives_counter     = max_lives;

        return true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        // Events are discarded when the scene is LOADING
        if (state == RUNNING)
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing ();                                                                 // Starts playing when the user touches the screen for the first time
            }
            else switch (event.id)
            {
                case ID(touch-started):                                                           // The user touches the screen
                {
                    // Determines the position where the screen has been touched
                    Point2f touch_position{ *event[ID(x)].as< var::Float > (),
                                            *event[ID(y)].as< var::Float > () };

                    // Checks if button is being pressed
                    if (pause_button->contains (touch_position))
                    {
                        director.run_scene (shared_ptr< Scene > (new Pause_Scene));               // Goes to the pause scene
                    }

                    // Checks if the point where the user touched is inside a food element
                    for (auto & item : food)
                    {
                        if (item->contains_point (touch_position))
                        {
                            // Adds another impulse to the food element
                            item->apply_impulse (food_touch_impulse);

                            // Adds to the score total
                            score_counter += item->get_points();
                        }
                    }

                    break;
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING:  load_textures  ();     break;
            case PREPARE:  get_ready      ();     break;
            case RUNNING:  run_simulation (time); break;
            case ERROR:   break;
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            // The canvas may have been created previously, in which case you just have to called it
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // If the canvas doesn't exist then is necessary to create it
            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ unsigned(canvas_width), unsigned(canvas_height) }});
            }

            // If the canvas is loaded or created then it is drawn
            if (canvas)
            {
                if (state == RUNNING) canvas->set_clear_color (0.f, 0.f, 0.f); else canvas->set_clear_color (1.f, 0.f, 0.f);

                canvas->clear ();

                if (state == RUNNING || state == PREPARE)
                {
                    // Draws the background image
                    canvas->fill_rectangle ({ 0.f, 0.f }, { canvas_width, canvas_height }, background.get (), Anchor::BOTTOM | Anchor::LEFT);

                    // Draws the food elements
                    for (auto & item : food)
                    {
                        item->render (*canvas);
                    }


                    if (state == PREPARE)
                    {
                        prepare     ->render (*canvas);                          // Draws the get ready sprite
                    }
                    else
                    {
                        life_icon   ->render (*canvas);                          // Draws the lives icon
                        score_icon  ->render (*canvas);                          // Draws the score icon
                        pause_button->render (*canvas);                          // Draws the pause button

                        wostringstream buffer_lifes;
                        wostringstream buffer_score;
                        wostringstream buffer_timer;

                        buffer_lifes << setfill (L'0');
                        buffer_lifes << lives_counter;

                        buffer_score << setfill (L'0');
                        buffer_score << score_counter;

                        buffer_timer << setfill (L'0');
                        buffer_timer << floor(game_timer.get_elapsed_seconds());

                        Text_Layout lives_text(*lives_font, buffer_lifes.str ());
                        Text_Layout score_text(*score_font, buffer_score.str ());
                        Text_Layout timer_text(*timer_font, buffer_timer.str ());

                        canvas->draw_text ({ life_icon->get_width(), canvas_height - 50.f }, lives_text, CENTER);                                       // Writes the lives counter
                        canvas->draw_text ({ score_icon->get_width() + life_icon->get_width() + 60.f , canvas_height - 50.f }, score_text, LEFT);       // Writes the score counter
                        canvas->draw_text ({ canvas_width / 2.f, canvas_height - 50.f }, timer_text, CENTER);                                           // Writes the game timer
                    }
                }

                if (gameplay == GAMEOVER)
                {
                    game_time_value = floor(game_timer.get_elapsed_seconds());

                    director.run_scene (shared_ptr< Scene > (new Gameover_Scene(score_counter, game_time_value)));                    // Goes to the gameover scene
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::load_textures ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Adjusts the aspect ratio for different screen sizes
            float real_aspect_ratio = float(context->get_surface_width()) / context->get_surface_height();

            canvas_width = unsigned(canvas_height * real_aspect_ratio);

            background      = Texture_2D::create (ID(background),      context, background_path);         // Loads the background texture
            prepare_texture = Texture_2D::create (ID(prepare_texture), context, prepare_path);            // Loads the get ready texture

            // Checks if the texture was loaded correctly
            if (background) {
                context->add (background);
                context->add (prepare_texture);
            }
            else
            {
                state = ERROR;
                return;
            }

            // Loads the get ready sprite
            prepare.reset (new Sprite(prepare_texture.get()));
            prepare->set_position ({ canvas_width / 2, canvas_height / 2 });

            // Loads the sprite atlas
            sprites_atlas.reset (new Atlas(sprites_atlas_path, context));

            if (!sprites_atlas->good ()) { state = ERROR; return; }

            // Loads the life icon
            life_icon.reset(new Sprite(sprites_atlas->get_slice (ID(life))));
            life_icon->set_position({ 50.f, canvas_height - 50.f });

            // Loads the score icon
            score_icon.reset(new Sprite(sprites_atlas->get_slice (ID(score))));
            score_icon->set_position({ life_icon->get_width() + 100.f, canvas_height - 50.f });

            // Loads the pause button
            pause_button.reset(new Sprite(sprites_atlas->get_slice (ID(pause))));
            pause_button->set_position({ canvas_width - 50.f, canvas_height - 50.f });

            // Creates the lives counter
            lives_font.reset (new Raster_Font(font_path, context));

            // Creates the score counter
            score_font.reset (new Raster_Font(font_path, context));

            // Creates the game timer
            timer_font.reset (new Raster_Font(font_path, context));

            state = PREPARE;

            game_timer.reset();
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::get_ready()
    {
        // The screen shows the get ready message for a few seconds
        if (game_timer.get_elapsed_seconds () > 2.f)
        {
            game_timer.reset ();

            create_sprites();

            state = RUNNING;
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {
        // Avoids that the 1st item will jump too fast
        if (time > 0.25f) return;

        // The position of the whole food elements is updated
        for (auto & item : food)
        {
            item->apply_force (float(gravity_force));       // Applies the gravity pull downwards
            item->update      (time);
        }

        if (spawn_timer.get_elapsed_seconds() >= spawn_delay)
        {
            create_pancake();
        }

        // If amount of spwan pancakes is over the limit then creates a strawberry
        if (launched_pancakes > limit_pancakes)
        {
            create_strawberry();                    // Creates one strawberry

            launched_pancakes = 0;                  // Resets launch pancake counter
        }

        // Checks if the food reached the bottom of the screen
        bool processed_all_items = false;

        while (!processed_all_items)
        {
            for (auto item = food.begin (); ; ++item)
            {
                if (item == food.end ())
                {
                    processed_all_items = true;
                    break;
                }

                // If a food element passes the bottom
                if ((*item)->get_position ().coordinates.y () < float(food_creation_bottom_y))
                {
                    food.erase (item);              // Erase food item

                    lives_counter--;                // Removes one life

                    if (lives_counter <= 0) {
                        lives_counter = 0;

                        gameplay = GAMEOVER;
                    }

                    break;
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::create_sprites ()
    {
        // Creates one pancake
        create_pancake();

        // Spawn pancake max limit for strawberry spawns
        limit_pancakes = rand() % (max_pancakes - min_pancakes) + min_pancakes;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------


    void Game_Scene::create_pancake()
    {
        shared_ptr< Pancake > pancake(new Pancake(sprites_atlas.get ()));

        float x = float(rand () % (int(canvas_width) - 100) + 100);
        float y = float(food_creation_bottom_y);

        pancake->set_position ({ x, y });
        pancake->apply_impulse (float(food_creation_impulse));

        food.push_back (pancake);

        launched_pancakes++;

        spawn_delay = float(rand () % (spawn_delay_max - spawn_delay_min) + spawn_delay_min);

        spawn_timer.reset();
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::create_strawberry()
    {
        shared_ptr< Strawberry > strawberry(new Strawberry(sprites_atlas.get ()));

        float x = float(rand () % (int(canvas_width) - 100) + 100);
        float y = float(food_creation_bottom_y);

        strawberry->set_position ({ x, y });
        strawberry->apply_impulse (float(food_creation_impulse));

        food.push_back (strawberry);

        // Calculates a new limit for the amount of pancakes launched to generate a new strawberry
        limit_pancakes = rand() % (max_pancakes - min_pancakes) + min_pancakes;
    }
}
