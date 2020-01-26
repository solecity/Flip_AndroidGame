/*
 * GAME SCENE
 * Copyright © 2020+ Mariana Moreira
 */

#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

    #include <map>
    #include <list>
    #include <memory>

    #include <basics/Canvas>
    #include <basics/Id>
    #include <basics/Scene>
    #include <basics/Texture_2D>
    #include <basics/Timer>

    #include "Sprite.hpp"
    #include "Food.hpp"

    using namespace basics;
    using namespace std;

    namespace flip
    {

        using basics::Id;
        using basics::Timer;
        using basics::Canvas;
        using basics::Graphics_Context;
        using basics::Texture_2D;

        class Game_Scene : public basics::Scene
        {

            /**
             * Represents the different scene states
             */
            enum State
            {
                LOADING,
                PREPARE,
                RUNNING,
                ERROR
            };

            /**
             * Represents the state of the game when the scene state is RUNNING
             */
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
                GAMEOVER,
            };

            static const int    max_lives              = 5;         ///< Maximum amount of lives a player can have

            static const int    food_creation_impulse  = 1500;      ///< Impulse of the food item when it is created
            static const int    food_creation_bottom_y =  -50;      ///< Position of y of the food element when it is created
            static const int    food_touch_impulse     = 1000;      ///< Impulse of the food item when the player touches one

            static const int    gravity_force          =  -30;      ///< Value of empirically estimated force of gravity

            static const int    min_pancakes           = 2;         ///< Minimum amount of pancakes to calculate when a strawberry appears
            static const int    max_pancakes           = 6;         ///< Maximum amount of pancakes to calculate when a strawberry appears

            static const char * background_path;                    ///< Path of the background texture
            static const char * prepare_path;                       ///< Path of the get ready texture
            static const char * sprites_atlas_path;                 ///< Path of the sprites atlas
            static const char * font_path;                          ///< Path of the font

            static const int    spawn_delay_min        = 1;        ///< Minimum amount of delay to launch a new pancake
            static const int    spawn_delay_max        = 5;        ///< Maximum amount of delay to launch a new pancake

        private:

            State          state;                                   ///< Scene state
            Gameplay_State gameplay;                                ///< Game state when the scene is RUNNING.

            bool           suspended;                               ///< true - when the scene is working on background and vice versa

            float          canvas_width;                            ///< Width of the window where the scene is drawn
            float          canvas_height;                           ///< Height of the window where the scene is drawn

            int            launched_pancakes = 0;                   ///< Amount of pancakes already on the screen
            int            limit_pancakes    = 0;                   ///< Limit of pancakes that need be launched for a strawberry to appear

            vector< std::shared_ptr< Food > > food;                 ///< Array with all the food elements

            shared_ptr< Texture_2D >  background;                   ///< Texture with the background image
            shared_ptr< Texture_2D >  prepare_texture;              ///< Texture with the get ready image

            shared_ptr< Sprite >      prepare;                      ///< Get ready sprite
            shared_ptr< Sprite >      life_icon;                    ///< Life sprite
            shared_ptr< Sprite >      score_icon;                   ///< Score sprite
            shared_ptr< Sprite >      pause_button;                 ///< Pause button sprite

            unique_ptr< Atlas >       sprites_atlas;                ///< Atlas that contains the images of all the game sprites

            unique_ptr< Raster_Font > lives_font;                   ///< Font to drawn the player lives
            unique_ptr< Raster_Font > score_font;                   ///< Font to drawn the game score
            unique_ptr< Raster_Font > timer_font;                   ///< Font to drawn the game timer

            Timer       game_timer;                                 ///< Timer used to measure the time in game
            Timer       spawn_timer;                                ///< Timer used to measure the time between food items being spwaned

            float       spawn_delay;                                ///< Amount of time require to pass between food being spwaned

            int 		lives_counter;                              ///< Amount of lives the player has
            int 		score_counter;                              ///< Current game score

        public:

            /**
             * Only initialize the attributes that must be initialized the first time when the scene is created from scratch.
             */
            Game_Scene ();

            /**
             * This method calls the Directory to know the screen resolution of the scene
             * @return size in coordinates that the scene is using
             */
            basics::Size2u get_view_size () override
            {
                return { unsigned(canvas_width), unsigned(canvas_height) };
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
             * This method loads the textures (one each frame to facilitate, so that the load itself can be paused when the application goes to the background)
             */
            void load_textures ();

            /**
             * This method calls all the sprites creating functions after all the textures are loaded
             */
            void create_sprites ();

            /**
             * This method creates the sprites of the pancakes
             */
            void create_pancake ();

            /**
             * This method creates the sprites of the strawberries
             */
            void create_strawberry ();

            /**
             * Draws the texture with the get ready message while the state of the scene is PREPARE
             */
            void get_ready ();

            /**
             * Changes the gamestate to PLAYING and the game starts
             */
            void start_playing ();

            /**
             * The game starts
             * The food elements start to show
             */
            void run_simulation (float time);

            /**
             * Changes the gamestate to GAME_OVER and the goes to the game over scene
             */
            void game_over ();

        };
    }

#endif
