/*
 * MENU SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"

using namespace basics;
using namespace std;

namespace flip
{
    const char * Menu_Scene::background_path = "game-scene/background.png";


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    Menu_Scene::Menu_Scene()
    {
        state         = LOADING;            // The scene starts with the LOADING state
        suspended     = true;
        canvas_width  = 1280;
        canvas_height =  720;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        for (auto & option : options)
        {
            option.is_pressed = false;      // All options are set as not pressed
        }

        return true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Menu_Scene::handle (basics::Event & event)
    {
        // Events are discarded when the scene is LOADING
        if (state == READY)
        {
            switch (event.id)
            {
                case ID(touch-started):         // The user touches the screen
                case ID(touch-moved):
                {
                    // Determines the position where the screen has been touched
                    Point2f touch_location = { *event[ID(x)].as< var::Float > (),
                                               *event[ID(y)].as< var::Float > () };

                    int     option_touched = option_at (touch_location);

                    // Only one option can be touched at a time (to avoid multiple selections), so only one is considered pressed (the rest are "released")
                    for (int index = 0; index < number_of_options; ++index)
                    {
                        options[index].is_pressed = index == option_touched;
                    }

                    break;
                }
                case ID(touch-ended):           // The user stops touching the screen
                {
                    // All options are "released"
                    for (auto & option : options) option.is_pressed = false;

                    // It determines which option was the last the user has stopped touching and acts accordingly
                    Point2f touch_location = { *event[ID(x)].as< var::Float > (),
                                               *event[ID(y)].as< var::Float > () };

                    if (option_at (touch_location) == PLAY)
                    {
                        director.run_scene (shared_ptr< Scene > (new Game_Scene));         // goes to the game scene
                    }

                    /*else if (option_at (touch_location) == HELP)
                    {
                        director.run_scene (shared_ptr< Scene > (new Help_Scene));         // goes to the help page
                    }
                    else if (option_at (touch_location) == CREDITS)
                    {
                        director.run_scene (shared_ptr< Scene > (new Credits_Scene));      // goes to the credits page
                    }
                     */

                    break;
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
    {
        if (!suspended) if (state == LOADING)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                // Loads the background texture
                background = Texture_2D::create (ID(background), context, background_path);

                context->add(background);

                // Loads the menu buttons atlas
                atlas.reset (new Atlas("menu-scene/buttons.sprites", context));

                // If the atlas could be loaded then the state is READY if not the is ERROR
                state = atlas->good () ? READY : ERROR;

                // If the atlas is available, the menu option data is initialized
                if (state == READY)
                {
                    configure_options ();
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Menu_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            // The canvas may have been created previously, in which case you just have to called it
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // If the canvas doesn't exist then is necessary to create it
            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // If the canvas is loaded or created then it is drawn
            if (canvas)
            {
                canvas->set_clear_color (255, 255, 255);
                canvas->clear ();

                if (state == READY)
                {
                    // Draws the background
                    canvas->fill_rectangle ({ 0.f, 0.f }, { (float) canvas_width, (float) canvas_height }, background.get (), Anchor::BOTTOM | Anchor::LEFT);

                    // The slice of each of the menu options is drawn
                    for (auto & option : options)
                    {
                        canvas->set_transform
                        (
                            scale_then_translate_2d
                            (
                                  option.is_pressed ? 0.75f : 1.f,              // Scales the option
                                { option.position[0], option.position[1] }      // Translation
                            )
                        );

                        canvas->fill_rectangle ({ 0.f, 0.f }, { option.slice->width, option.slice->height }, option.slice, CENTER | TOP);
                    }

                    // The transformation applied to the options is restored so it does not affect subsequent drawings made with the same canvas
                    canvas->set_transform (Transformation2f());
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::configure_options ()
    {
        // A slice of the atlas is assigned to each menu option according to its ID
        options[PLAY   ].slice = atlas->get_slice (ID(play)   );
        options[HELP   ].slice = atlas->get_slice (ID(help)   );
        options[CREDITS].slice = atlas->get_slice (ID(credits));

        // The total height of the menu is calculated
        float menu_height = 0;

        for (auto & option : options) menu_height += option.slice->height;

        // The position of the top edge of the menu as a whole is calculated so that it is centered vertically
        float option_top = canvas_height / 2.f + menu_height / 2.f - 50.f;

        // The position of the top edge of each option is set
        for (unsigned index = 0; index < number_of_options; ++index)
        {
            options[index].position = Point2f{ canvas_width / 2.f, option_top };

            option_top -= options[index].slice->height;
        }

        // The pressure of each option is restored
        initialize ();
    }

    // ---------------------------------------------------------------------------------------------

    int Menu_Scene::option_at (const Point2f & point)
    {
        for (int index = 0; index < number_of_options; ++index)
        {
            const Option & option = options[index];

            if
            (
                point[0] > option.position[0] - option.slice->width  &&
                point[0] < option.position[0] + option.slice->width  &&
                point[1] > option.position[1] - option.slice->height &&
                point[1] < option.position[1] + option.slice->height
            )
            {
                return index;
            }
        }

        return -1;
    }

}
