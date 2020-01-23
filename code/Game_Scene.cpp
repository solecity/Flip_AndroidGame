/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Game_Scene.hpp"
#include "Pancake.hpp"

#include <cstdlib>
#include <basics/Log>
#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace flip
{

    const char * Game_Scene::background_path = "game-scene/background.png";
    //const char * Game_Scene::prepare_path    = "game-scene/get_ready.png";
    const char * Game_Scene::food_atlas_path = "game-scene/food.sprites";
    //const char * Game_Scene::food_atlas_path = "game-scene/pancakes.sprites";


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        // Se establece la resolución virtual (independiente de la resolución virtual del dispositivo).
        // En este caso no se hace ajuste de aspect ratio, por lo que puede haber distorsión cuando
        // el aspect ratio real de la pantalla del dispositivo es distinto.

        canvas_width  = 1280;
        canvas_height =  720;

        // Starts the random number generator seed
        srand (unsigned(time(nullptr)));

        // Other attributes are initialized
        initialize ();

        suspended = true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Algunos atributos se inicializan en este método en lugar de hacerlo en el constructor porque
    // este método puede ser llamado más veces para restablecer el estado de la escena y el constructor
    // solo se invoca una vez.

    bool Game_Scene::initialize ()
    {
        state     = background.get () ? RUNNING : LOADING;
        suspended = false;
        gameplay  = UNINITIALIZED;

        score     = 0;
        lives     = max_lives;

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
                start_playing ();           // Starts playing when the user touches the screen for the first time
            }
            else switch (event.id)
            {
                case ID(touch-started):     // The user touches the screen
                {
                    // Determines the position where the screen has been touched
                    Point2f touch_position{ *event[ID(x)].as< var::Float > (),
                                            *event[ID(y)].as< var::Float > () };

                    // Checks if the point where the user touched is inside a food element
                    for (auto & item : food)
                    {
                        if (item->contains_point (touch_position))
                        {
                            // Adds another impulse to the food element
                            item->apply_impulse (food_touch_impulse);

                            // Adds to the score total
                                //score += item->get_points();
                        }
                    }


                    // ---------------------------------------------------------
                    // REMOVER
                    // Esto es para hacer pruebas. Se debe quitar:
                        create_pancake ();

                    // VERIFICAR SE O MENU CONTEM O TOUCH POSITION
                    // SE CLICAR NO MENU PAUSA
                        //director.run_scene (shared_ptr< Scene >(new Pause_Scene));
                    // ---------------------------------------------------------

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
            case LOADING: load_textures  ();     break;
            case PREPARE: get_ready      ();     break;
            case RUNNING: run_simulation (time); break;
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

                    // ---------------------------------------------------------
                    // DESENHAR
                        // BOTAO PAUSA
                        // VIDA
                        // TIMER
                    // ---------------------------------------------------------

                    if (state == PREPARE)
                    {
                        // ---------------------------------------------------------
                        // DESENHAR TEXTURA "get ready"
                        // ---------------------------------------------------------
                    }
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // In this method, only one texture is loaded per frame to pause the load if the game changes to second plane unexpectedly
    // Another  aspect is that the load doesn't start until the scene starts to have the possibility to show the user
    // that the load is in progress instead of having a black screen that does not respond during a time.

    void Game_Scene::load_textures ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // ---------------------------------------------------------
            // hpp : shared_ptr<Texture_2D> pages[3];       // ??????????????????????

            // AJUSTAR ASPECT RATION -- EX ASTEROIDS
            // ---------------------------------------------------------


            // Loads the background texture
            background = Texture_2D::create (ID(background), context, background_path);

            // TEXTURA get ready
            // The get ready texture is loaded
                //prepare = Texture_2D::create (ID(prepare), context, prepare_path);

            // Se comprueba si la textura se ha podido cargar correctamente:
            if (background) context->add (background); else { state = ERROR; return; }



            // ---------------------------------------------------------
            // TEXTURA - MUDAR FOOD ATLAS PARA UM GERAL
                // BOTAO PAUSA
                // VIDA
                // PANQUECAS
                // MORANGO
            // ---------------------------------------------------------

            // Loads the food atlas
            food_atlas.reset (new Atlas(food_atlas_path, context));

            if (!food_atlas->good ()) { state = ERROR; return; }


            // ---------------------------------------------------------
            // RASTER FONT - PARA OS NUMEROS DO TIMER
            // ---------------------------------------------------------

            create_sprites();

            state = PREPARE;
            timer.reset();
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Game_Scene::get_ready()
    {
        // The screen shows the get ready message for a few seconds
        if (timer.get_elapsed_seconds () > 2.f)
        {
            timer.reset ();

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
        if (time > 0.25F) return;

        // The position of the whole food elemetns is updated
        for (auto & item : food)
        {
            item->apply_force (float(gravity_force));       // Applies the gravity pull downwards
            item->update      (time);
        }

        // ---------------------------------------------------------
        // CREAR BONUS
        // SE O LIMITE DE PANQUECAS NO ECRA FOR ATINGIDO GERA UM MORANGO
        // RESET DO LIMITE DE PANQUECAS

        /* if (limit_pancakes >= ....)
        {
            create_strawberry();
            launched_pancakes = 0;
        }
        */
        // ---------------------------------------------------------

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

                if ((*item)->get_position ().coordinates.y () < float(food_creation_bottom_y))
                {
                    basics::log.d ("La comida llegó abajo...");

                    food.erase (item);

                    // ---------------------------------------------------------
                    // REMOVER VIDA
                    // ---------------------------------------------------------

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

        // ---------------------------------------------------------
        // LIMITE DE PANQUECAS PARA MORANGO
            // Sets the limit for how many pancakes need to show until a strawberry appears
            //limit_pancakes = rand() % (max - min) + min

        // BOTAO PAUSA
            // Creates the pause menu button

        // TIMER
            // Creates the game timer

        // VIDAS
            // Creates the player lives images
        // ---------------------------------------------------------
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------


    void Game_Scene::create_pancake()
    {
        shared_ptr< Pancake > pancake(new Pancake(food_atlas.get ()));

        float x = float(rand () % int(canvas_width));
        float y = float(food_creation_bottom_y);

        pancake->set_position ({ x, y });
        pancake->apply_impulse (float(food_creation_impulse));

        food.push_back (pancake);

        launched_pancakes++;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    // creates one strawberry

    void Game_Scene::create_strawberry() {

        /*
        shared_ptr< Strawberry > strawberry(new Strawberry(food_atlas.get ()));

        float x = float(rand () % int(canvas_width));
        float y = float(food_creation_bottom_y);

        strawberry->set_position ({ x, y });
        strawberry->apply_impulse (float(food_creation_impulse));
        food.push_back (strawberry);

        // Calculates a new limit for the amount of pancakes launched to generate a new straberry
        // limit_pancakes = rand() % (max - min) + min
        */
    }
}
