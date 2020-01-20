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

namespace example
{

    const char * Game_Scene::background_path = "game-scene/background.png";
    const char * Game_Scene::food_atlas_path = "game-scene/food.sprites";
    //const char * Game_Scene::food_atlas_path = "game-scene/pancakes.sprites";

    // ---------------------------------------------------------------------------------------------

    Game_Scene::Game_Scene()
    {
        // Se establece la resolución virtual (independiente de la resolución virtual del dispositivo).
        // En este caso no se hace ajuste de aspect ratio, por lo que puede haber distorsión cuando
        // el aspect ratio real de la pantalla del dispositivo es distinto.

        canvas_width  = 1280;
        canvas_height =  720;

        // Se inicia la semilla del generador de números aleatorios:

        srand (unsigned(time(nullptr)));

        // Se inicializan otros atributos:

        initialize ();

        suspended = true;
    }

    // ---------------------------------------------------------------------------------------------
    // Algunos atributos se inicializan en este método en lugar de hacerlo en el constructor porque
    // este método puede ser llamado más veces para restablecer el estado de la escena y el constructor
    // solo se invoca una vez.

    bool Game_Scene::initialize ()
    {
        state     = background.get () ? RUNNING : LOADING;
        suspended = false;
        gameplay  = UNINITIALIZED;

        score = 0;
        lives = max_lives;

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::suspend ()
    {
        suspended = true;               // Se marca que la escena ha pasado a primer plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::resume ()
    {
        suspended = false;              // Se marca que la escena ha pasado a segundo plano
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)               // Se descartan los eventos cuando la escena está LOADING
        {
            if (gameplay == WAITING_TO_START)
            {
                start_playing ();           // Se empieza a jugar cuando el usuario toca la pantalla por primera vez
            }
            else switch (event.id)
            {
                case ID(touch-started):     // El usuario toca la pantalla
                {
                    // Se determina dónde se ha tocado en la pantalla:

                    Point2f touch_position{ *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    // Se comprueba si el punto donde se ha tocado queda dentro de alguna comida:

                    for (auto & item : food)
                    {
                        if (item->contains_point (touch_position))
                        {
                            item->apply_impulse (food_touch_impulse);

                            // score total
                            score += item->get_points();
                        }
                    }

                    // Esto es para hacer pruebas. Se debe quitar:

                    create_pancake ();

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING: load_textures  ();     break;
            case RUNNING: run_simulation (time); break;
            case ERROR:   break;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ unsigned(canvas_width), unsigned(canvas_height) }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                if (state == RUNNING) canvas->set_clear_color (0.f, 0.f, 0.f); else canvas->set_clear_color (1.f, 0.f, 0.f);

                canvas->clear ();

                if (state == RUNNING)
                {
                    canvas->fill_rectangle ({ 0.f, 0.f }, { canvas_width, canvas_height }, background.get (), Anchor::BOTTOM | Anchor::LEFT);

                    for (auto & item : food)
                    {
                        item->render (*canvas);
                    }
                    // DESENHAR
                    // MENU PAUSA
                    // VIDA
                    // TIMER
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------
    // En este método solo se carga una textura por fotograma para poder pausar la carga si el
    // juego pasa a segundo plano inesperadamente. Otro aspecto interesante es que la carga no
    // comienza hasta que la escena se inicia para así tener la posibilidad de mostrar al usuario
    // que la carga está en curso en lugar de tener una pantalla en negro que no responde durante
    // un tiempo.

    void Game_Scene::load_textures ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {

            // ajustar aspect ration -- ex asteroids

            // Se carga la siguiente textura (textures.size() indica cuántas llevamos cargadas):
            background = Texture_2D::create (ID(background), context, background_path);

            // Se comprueba si la textura se ha podido cargar correctamente:
            if (background) context->add (background); else { state = ERROR; return; }

            // ---


            // TEXTURE - mudar food atlas para geral
            // menu pausa
            // vida
            // pancakes
            // bonus

            // food texture
            food_atlas.reset (new Atlas(food_atlas_path, context));

            if (!food_atlas->good ()) { state = ERROR; return; }

            // ---

            // raster font - numeros timer


            // ---

            create_sprites();

            state = RUNNING;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::start_playing ()
    {
        gameplay = PLAYING;
    }

    // ---------------------------------------------------------------------------------------------

    void Game_Scene::run_simulation (float time)
    {
        // avoid the 1st item to jump too fast
        if (time > 0.25F) return;

        // Se actualiza la posición de toda la comida:
        for (auto & item : food)
        {
            item->apply_force (float(gravity_force));
            item->update      (time);
        }


        // create bonus
        // if limit_pancakes >= ....
            // create_strawberry();
            // launched_pancakes = 0;


        // VERIFICACAO
        // Se comprueba si la comida llega a la parte inferior de la pantalla:
        //
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

                    // HACER ALGUNA OTRA COSA CUANDO LA COMIDA LLEGA ABAJO

                    break;
                }
            }
        }
        //
    }


    // ---------------------------------------------------------------------------------------------

    void Game_Scene::create_sprites ()
    {
        // Se crean y configuran los sprites del fondo:

        // panquecas
        create_pancake();

        // limit_pancakes = rand() % (max - min) + min


        // menu pausa

        // timer

        // vidas
    }


    // ---------------------------------------------------------------------------------------------


    // cria uma panqueca
    void Game_Scene::create_pancake()
    {
        shared_ptr< Pancake > pancake(new Pancake(food_atlas.get ()));

        float x = float(rand () % int(canvas_width));
        float y = float(food_creation_bottom_y);

        pancake->set_position ({ x, y });
        pancake->set_scale ( 2.f );

        pancake->apply_impulse (float(food_creation_impulse));

        food.push_back (pancake);

        launched_pancakes++;
    }


    // bonus
    void Game_Scene::create_strawberry() {

        // limit_pancakes = rand() % (max - min) + min

        // crear bonus elemento
    }

}
