/*
 * FOOD CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#include "Food.hpp"

using namespace basics;
using namespace std;

namespace flip
{

    Food::Food(Atlas * atlas, float radius) : radius(radius)
    {
        this->atlas = atlas;

        position = { 0.f, 0.f };
        speed    = { 0.f, 0.f };
        size     = { 0.f, 0.f };
        anchor   = CENTER;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Food::update(float time)
    {
        position.coordinates.x () += speed.coordinates.x () * time;
        position.coordinates.y () += speed.coordinates.y () * time;
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Food::render(Canvas & canvas)
    {
        // The appropriate frame is searched depending on the number of frames available for the animation of ascending or falling and of the speed at which the food is moving
        const Atlas::Slice * animation_slice = nullptr;     // It is not yet known which frame it will be

        float speed_y = speed.coordinates.y ();

        if (speed_y > 0.f)                                  // The food element is ascending
        {
            int index = ascending_keyframes.size ();

            for (float range = max_speed, step = max_speed / float(ascending_keyframes.size ()); range > 0.f; range -= step)
            {
                index--;

                if (speed_y > range)
                {
                    animation_slice = atlas->get_slice (ascending_keyframes[index]);
                    break;
                }
            }
        }
        else                                                // The food element is falling
        {
            int index = falling_keyframes.size ();

            for (float range = -max_speed, step = max_speed / float(falling_keyframes.size ()); range < 0.f; range += step)
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
            size[0] = animation_slice->width;
            size[1] = animation_slice->height;

            /*
            // Draws the collider
            canvas.set_color (0.f, 1.f, 0.f);
            canvas.draw_rectangle({ position[0] - radius, position[1] - radius }, { radius * 2.f, radius * 2.f });
             */

            // Draws the food element
            canvas.fill_rectangle
            (
                  position,
                { animation_slice->width, animation_slice->height },
                  animation_slice,
                  Anchor::CENTER
            );
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Food::contains_point(const Point2f & point)
    {
        // calculates if the user hits the collider circle
        float  distance = (Vector2f(position.coordinates) - Vector2f(point.coordinates)).length ();

        return distance < radius;
    }
}
