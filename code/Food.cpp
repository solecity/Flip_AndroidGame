
// comentario


#include "Food.hpp"

using namespace basics;
using namespace std;

namespace example
{

    Food::Food(Atlas * atlas)
    {
        this->atlas = atlas;

        position = { 0.f, 0.f };
        speed    = { 0.f, 0.f };
    }

    void Food::update(float time)
    {
        position.coordinates.x () += speed.coordinates.x () * time;
        position.coordinates.y () += speed.coordinates.y () * time;
    }

    void Food::render(Canvas & canvas)
    {
        // Se busca el fotograma apropiado dependiendo del número de fotogramas que haya para la
        // animación de subir o bajar y de la velocidad a la que se esté moviendo la comida:

        const Atlas::Slice * animation_slice = nullptr;     // Todavía no se sabe cuál será el fotograma

        float speed_y = speed.coordinates.y ();

        if (speed_y > 0.f)      // Está subiendo
        {
            int index = ascending_keyframes.size ();

            for (float range = max_speed, step = max_speed / float(ascending_keyframes.size ()); range > 0.F; range -= step)
            {
                index--;

                if (speed_y > range)
                {
                    animation_slice = atlas->get_slice (ascending_keyframes[index]);
                    break;
                }
            }
        }
        else                    // Está bajando
        {
            int index = falling_keyframes.size ();

            for (float range = -max_speed, step = max_speed / float(falling_keyframes.size ()); range < 0.F; range += step)
            {
                index--;

                if (speed_y < range)
                {
                    animation_slice = atlas->get_slice (falling_keyframes[index]);
                    break;
                }
            }
        }

        if (animation_slice)
        {
            canvas.fill_rectangle
            (
                position,
                { animation_slice->width * scale, animation_slice->height * scale },
                animation_slice,
                Anchor::CENTER
            );
        }
    }

    bool Food::contains_point(const Point2f &point)
    {
        // Mirar la clase Sprite para determinar cómo implementar este método
        return false;
    }

}
