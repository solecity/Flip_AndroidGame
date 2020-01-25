/*
 * INTRO SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#include <basics/Canvas>
#include <basics/Director>

#include "Intro_Scene.hpp"
#include "Menu_Scene.hpp"

using namespace basics;
using namespace std;

namespace flip
{

    Intro_Scene::Intro_Scene ()
    {
        state         = UNINITIALIZED;

        canvas_width  = 1280;
        canvas_height =  720;

        suspended     = true;
    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Intro_Scene::initialize ()
    {
        if (state == UNINITIALIZED)
        {
            state = LOADING;
        }
        else
        {
            timer.reset ();

            opacity = 0.f;
            state   = FADING_IN;
        }

        suspended   = false;

        return true;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING:    update_loading    (); break;
            case FADING_IN:  update_fading_in  (); break;
            case WAITING:    update_waiting    (); break;
            case FADING_OUT: update_fading_out (); break;
            default: break;
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::render (Graphics_Context::Accessor & context)
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

                if (title_texture)
                {
                    canvas->set_opacity (opacity);

                    canvas->fill_rectangle
                    (
                        { canvas_width * .5f, canvas_height * .5f },
                        { title_texture->get_width (), title_texture->get_height () },
                          title_texture. get ()
                    );
                }
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::update_loading ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Adjusts the aspect ratio for different screen sizes
            float real_aspect_ratio = float(context->get_surface_width()) / context->get_surface_height();

            canvas_width = unsigned(canvas_height * real_aspect_ratio);

            // Texture of the title image
            title_texture = Texture_2D::create (0, context, "title.png");

            // Checks if the title texture was correctly loaded
            if (title_texture)
            {
                context->add (title_texture);

                timer.reset ();

                opacity = 0.f;
                state   = FADING_IN;
            }
            else
            {
                state   = ERROR;
            }
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::update_fading_in ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < 1.f)
        {
            opacity = elapsed_seconds;      // The opacity of the title image is increased slowly as time passes
        }
        else
        {
            timer.reset ();

            opacity = 1.f;
            state   = WAITING;
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::update_waiting ()
    {
        // The scene does nothing for 2 seconds
        if (timer.get_elapsed_seconds () > 2.f)
        {
            timer.reset ();

            state = FADING_OUT;
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Intro_Scene::update_fading_out ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < .5f)
        {
            opacity = 1.f - elapsed_seconds * 2.f;      // The opacity is reduced from 1 to 0 in half a second
        }
        else
        {
            // When the fade out is complete, the menu scene is loaded
            state   = FINISHED;

            director.run_scene (shared_ptr< Scene > (new Menu_Scene));
        }
    }
}
