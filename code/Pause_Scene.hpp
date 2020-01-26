/*
 * PAUSE SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#ifndef PAUSE_SCENE_HEADER
#define PAUSE_SCENE_HEADER

    #include <memory>

    #include <basics/Atlas>
    #include <basics/Canvas>
    #include <basics/Point>
    #include <basics/Scene>
    #include <basics/Size>
    #include <basics/Texture_2D>

    #include "Sprite.hpp"

    using namespace basics;
    using namespace std;

    namespace flip
    {

        using basics::Atlas;
        using basics::Canvas;
        using basics::Point2f;
        using basics::Size2f;
        using basics::Texture_2D;
        using basics::Graphics_Context;

        class Pause_Scene : public basics::Scene
        {

            /**
             *  Represents the different scene states.
             */
            enum State
            {
                LOADING,
                READY,
                ERROR
            };

            /**
             *  Represents the different ids of the menu buttons
             */
            enum Option_Id
            {
                PLAY_AGAIN
            };

            /**
             *  Represents the menu option structure
             */
            struct Option
            {
                const Atlas::Slice * slice;
                Point2f              position;
                float                is_pressed;
            };

            static const char *   pause_path;                   ///< Path of the pause texture
            static const char *   button_path;                  ///< Path of the home button
            static const char *   buttons_atlas_path;           ///< Path of the buttons atlas

            static const unsigned number_of_options = 1;        ///< Amount of options in the menu

        private:

            State    state;                                     ///< Scene state

            bool     suspended;                                 ///< true - when the scene is working on background and vice versa

            unsigned canvas_width;                              ///< Width of the window where the scene is drawn
            unsigned canvas_height;                             ///< Height of the window where the scene is drawn

            Option   options[number_of_options];                ///< Menu options data

            shared_ptr< Texture_2D > pause_texture;             ///< Texture with the pause image
            shared_ptr< Texture_2D > button_texture;            ///< Texture with the home button image

            shared_ptr< Sprite >     home_button;               ///< Home button sprite

            unique_ptr< Atlas >      button_atlas;              ///< Atlas with the menu options images

        public:

            /**
             * Sets the initial values of the scene when it is first loaded
             */
            Pause_Scene ();

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

        private:

            /**
             * Set the properties of each option if the atlas could be loaded.
             */
            void configure_options ();

            /**
             * Returns the index of the option that is below the indicated point.
             * @param point Point that will be used to determine which option is below.
             * @return Index of the option that is below the point or -1 if there is none.
             */
            int option_at (const Point2f & point);

        };
    }

#endif
