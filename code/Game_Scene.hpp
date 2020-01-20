/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
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

    //#include "Sprite.hpp"
    #include "Food.hpp"

    using namespace basics;
    using namespace std;

    namespace example
    {

        using basics::Id;
        using basics::Timer;
        using basics::Canvas;
        using basics::Graphics_Context;
        using basics::Texture_2D;

        class Game_Scene : public basics::Scene
        {
            /**
             * Representa el estado de la escena en su conjunto.
             */
            enum State
            {
                LOADING,
                RUNNING,
                ERROR
            };

            /**
             * Representa el estado del juego cuando el estado de la escena es RUNNING.
             */
            enum Gameplay_State
            {
                UNINITIALIZED,
                WAITING_TO_START,
                PLAYING,
                GAME_OVER,
            };

            static const int max_lives     = 3;

            static const int food_creation_impulse  = 1500;
            static const int food_creation_bottom_y =  -50;     // first position where generated
            static const int food_touch_impulse     = 1000;

            static const int gravity_force          =  -30;       ///< Valor de la fuerza de gravedad estimado empíricamente

            static const int min_pancakes = 10;
            static const int max_pancakes = 20;


            static const char * background_path;
            static const char * food_atlas_path;

        private:

            State          state;                               ///< Estado de la escena.
            Gameplay_State gameplay;                            ///< Estado del juego cuando la escena está RUNNING.
            bool           suspended;                           ///< true cuando la escena está en segundo plano y viceversa.

            float          canvas_width;                        ///< Ancho de la resolución virtual usada para dibujar.
            float          canvas_height;                       ///< Alto  de la resolución virtual usada para dibujar.

            int            launched_pancakes = 0;
            int            limit_pancakes    = 0;

            struct Ui_Image
            {
                Atlas::Slice * slice;
                Point2f        position;
                Anchor         anchor;
            };

            vector< std::shared_ptr< Food > > food;

            shared_ptr< Texture_2D > background;

            unique_ptr< Atlas > user_interface_atlas;
            unique_ptr< Atlas > food_atlas;

            unique_ptr< Raster_Font > font;

            int 		score;

            Timer       timer;                               ///< Cronómetro usado para medir intervalos de tiempo

            int 		lives;

            Ui_Image    menu_button;
            Ui_Image    game_over;


        public:

            /**
             * Solo inicializa los atributos que deben estar inicializados la primera vez, cuando se
             * crea la escena desde cero.
             */
            Game_Scene();

            /**
             * Este método lo llama Director para conocer la resolución virtual con la que está
             * trabajando la escena.
             * @return Tamaño en coordenadas virtuales que está usando la escena.
             */
            basics::Size2u get_view_size () override
            {
                return { unsigned(canvas_width), unsigned(canvas_height) };
            }

            /**
             * Aquí se inicializan los atributos que deben restablecerse cada vez que se inicia la escena.
             * @return
             */
            bool initialize () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a segundo plano.
             */
            void suspend () override;

            /**
             * Este método lo invoca Director automáticamente cuando el juego pasa a primer plano.
             */
            void resume () override;

            /**
             * Este método se invoca automáticamente una vez por fotograma cuando se acumulan
             * eventos dirigidos a la escena.
             */
            void handle (basics::Event & event) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * actualize su estado.
             */
            void update (float time) override;

            /**
             * Este método se invoca automáticamente una vez por fotograma para que la escena
             * dibuje su contenido.
             */
            void render (Graphics_Context::Accessor & context) override;

        private:

            /**
             * En este método se cargan las texturas (una cada fotograma para facilitar que la
             * propia carga se pueda pausar cuando la aplicación pasa a segundo plano).
             */
            void load_textures ();

            /**
             * En este método se crean los sprites cuando termina la carga de texturas.
             */
            void create_sprites ();

            /**
             * Cuando se ha reiniciado el juego y el usuario toca la pantalla por primera vez se
             * pone la bola en movimiento en una dirección al azar.
             */
            void start_playing ();

            /**
             * Actualiza el estado del juego cuando el estado de la escena es RUNNING.
             */
            void run_simulation (float time);

            void create_pancake ();

        };

    }

#endif
