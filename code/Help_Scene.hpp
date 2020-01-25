/*
 * MENU SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#ifndef HELP_SCENE_HEADER
#define HELP_SCENE_HEADER

    #include <memory>

    #include <basics/Atlas>
    #include <basics/Canvas>
    #include <basics/Point>
    #include <basics/Scene>
    #include <basics/Size>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    #include "Sprite.hpp"

    using namespace basics;
    using namespace std;

    namespace flip
    {

        using basics::Atlas;
        using basics::Canvas;
        using basics::Point2f;
        using basics::Size2f;
        using basics::Graphics_Context;
        using basics::Texture_2D;

        class Help_Scene : public basics::Scene
        {

            /**
             *  Represents the different scene states.
             */
            enum State
            {
                LOADING,
                READY
            };

            static const char * help_path;                        ///< Path of the help texture
            static const char * button_path;                    ///< Path of the home button

        private:

            State    state;                                     ///< Scene state

            bool     suspended;                                 ///< true - when the scene is working on background and vice versa

            unsigned canvas_width;                              ///< Width of the window where the scene is drawn
            unsigned canvas_height;                             ///< Height of the window where the scene is drawn

            shared_ptr< Texture_2D > help_texture;              ///< Texture with the help image
            shared_ptr< Texture_2D > button_texture;            ///< Texture with the home button image

            shared_ptr< Sprite >     home_button;               ///< Home button sprite

        public:

            /**
             * Only initialize the attributes that must be initialized the first time when the scene is created from scratch.
             */
            Help_Scene ();

            /**
             * This method calls the Directory to know the screen resolution of the scene
             * @return - size in coordinates that the scene is using
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
             * This method is automatically invoked once per frame when events directed to the scene are accumulated.
             */
            void handle (basics::Event & event) override;

            /**
             * This method is automatically invoked once every frame so the scene can update its state
             */
            void update (float time) override;

            /**
             * This method is automatically invoked once every frame so the scene draws its content
             */
            void render (Graphics_Context::Accessor & context) override;

        };
    }

#endif
