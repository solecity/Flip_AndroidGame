/*
 * MENU SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

#include "Help_Scene.hpp"
#include "Menu_Scene.hpp"

using namespace basics;
using namespace std;

namespace flip
{
    const char * Help_Scene::help_path         = "menu-scene/help.png";
    const char * Help_Scene::button_path     = "menu-scene/home.png";


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    Help_Scene::Help_Scene ()
    {
        state         = LOADING;            // The scene starts with the LOADING state

        canvas_width  = 1280;
        canvas_height =  720;

        suspended     = true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Help_Scene::initialize ()
    {
        suspended = false;

        return true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Help_Scene::handle (basics::Event & event)
    {
        // Events are discarded when the scene is LOADING
        if (state == READY)
        {
            switch (event.id)
            {
                case ID(touch-started):                                                          // The user touches the screen
                {
                    // Determines the position where the screen has been touched
                    Point2f touch_position = { *event[ID(x)].as< var::Float > (),
                                               *event[ID(y)].as< var::Float > () };

                    // Checks if button is being pressed
                    if (home_button->contains (touch_position))
                    {
                        director.run_scene (shared_ptr< Scene > (new Menu_Scene));               // Returns to the menu scene
                    }

                    break;
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Help_Scene::update (float time)
    {
        if (!suspended) if (state == LOADING)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                // Adjusts the aspect ratio for different screen sizes
                float real_aspect_ratio = float(context->get_surface_width()) / context->get_surface_height();

                canvas_width = unsigned(canvas_height * real_aspect_ratio);

                help_texture   = Texture_2D::create (ID(help_texture),   context, help_path);           // Loads the help texture
                button_texture = Texture_2D::create (ID(button_texture), context, button_path);         // Loads the button texture

                context->add(help_texture);
                context->add(button_texture);

                // button sprite
                home_button.reset(new Sprite(button_texture.get()));
                home_button->set_position({ canvas_width - 50.f, canvas_height - 50.f });

                state = READY;
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Help_Scene::render (Graphics_Context::Accessor & context)
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
                    canvas->fill_rectangle ({ 0.f, 0.f }, { (float) canvas_width, (float) canvas_height }, help_texture.get (), Anchor::BOTTOM | Anchor::LEFT);

                    home_button->render(*canvas);
                }
            }
        }
    }
}
