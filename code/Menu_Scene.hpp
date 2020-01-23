/*
 * MENU SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#ifndef MENU_SCENE_HEADER
#define MENU_SCENE_HEADER

    #include <memory>

    #include <basics/Atlas>
    #include <basics/Canvas>
    #include <basics/Point>
    #include <basics/Scene>
    #include <basics/Size>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    using namespace basics;
    using namespace std;

    namespace flip
    {

        using basics::Atlas;
        using basics::Timer;
        using basics::Canvas;
        using basics::Point2f;
        using basics::Size2f;
        using basics::Graphics_Context;
        using basics::Texture_2D;

        class Menu_Scene : public basics::Scene
        {

            /**
             *  Represents the different scene states.
             */
            enum State
            {
                LOADING,
                READY,
                FINISHED,
                ERROR
            };

            /**
             *  Represents the different ids of the menu buttons
             */
            enum Option_Id
            {
                PLAY,
                HELP,
                CREDITS
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

            static const char * background_path;                ///< Path of the background texture

            static const unsigned number_of_options = 3;        ///< Amount of options in the menu

        private:

            State    state;                                     ///< Scene state

            bool     suspended;                                 ///< true - when the scene is working on background and vice versa

            unsigned canvas_width;                              ///< Width of the window where the scene is drawn
            unsigned canvas_height;                             ///< Height of the window where the scene is drawn

            Timer    timer;                                     ///< Timer used to measure time intervals

            Option   options[number_of_options];                ///< Menu options data

            shared_ptr< Texture_2D > background;                ///< Texture with the background image

            unique_ptr< Atlas > atlas;                          ///< Atlas that contains the images of the menu options

        public:

            /**
             * Only initialize the attributes that must be initialized the first time when the scene is created from scratch.
             */
            Menu_Scene();

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

        private:

            /**
             * Set the properties of each option if the atlas could be loaded.
             */
            void configure_options ();

            /**
             * Returns the index of the option that is below the indicated point.
             * @param point - Point that will be used to determine which option is below.
             * @return - Index of the option that is below the point or -1 if there is none.
             */
            int option_at (const Point2f & point);

        };
    }

#endif
