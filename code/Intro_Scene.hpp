/*
 * INTRO SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#ifndef INTRO_SCENE_HEADER
#define INTRO_SCENE_HEADER

    #include <memory>

    #include <basics/Canvas>
    #include <basics/Scene>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    namespace flip
    {

        using basics::Timer;
        using basics::Canvas;
        using basics::Texture_2D;
        using basics::Graphics_Context;

        class Intro_Scene : public basics::Scene
        {

            /**
             * Represents the different scene states
             */
            enum State
            {
                UNINITIALIZED,
                LOADING,
                FADING_IN,
                WAITING,
                FADING_OUT,
                FINISHED,
                ERROR
            };

        private:

            State    state;                                     ///< Scene state

            bool     suspended;                                 ///< true - when the scene is working on background and vice versa

            unsigned canvas_width;                              ///< Width of the window where the scene is drawn
            unsigned canvas_height;                             ///< Height of the window where the scene is drawn

            Timer    timer;                                     ///< Timer used to measure time intervals

            float    opacity;                                   ///< Opacity of the texture

            std::shared_ptr<Texture_2D> title_texture;          ///< Texture with the title image

        public:

            /**
             * Sets the initial values of the scene when it is first loaded
             */
            Intro_Scene ();

            /**
             * This method calls the Directory to know the screen resolution of the scene
             * @return size in coordinates that the scene is using
             */
            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            /**
             * Initiate all attributes that need to be loaded every time this scene starts
             * @return
             */
            bool initialize () override;

            /**
             * This method calls the Directory when the scene changes to second plan
             */
            void suspend () override
            {
                suspended = true;
            }

            /**
             * This method calls the Directory when the scene changes to first plan
             */
            void resume () override
            {
                suspended = false;
            }

            /**
             * This method is invoked automatically once every frame so the scene can update its state
             */
            void update (float time) override;

            /**
             * This method is invoked automatically once every frame so the scene draws its content
             */
            void render (Graphics_Context::Accessor & context) override;

        private:

            void update_loading    ();              ///< Loads the content of the scene
            void update_fading_in  ();              ///< Applies a fade in effect
            void update_waiting    ();              ///< Prevents the scene from changing for a time
            void update_fading_out ();              ///< Applies a fade out effect

        };
    }

#endif
