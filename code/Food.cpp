/*
 * FOOD CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#include "Food.hpp"

using namespace basics;
using namespace std;

namespace flip
{

    Food::Food(Atlas * atlas)
    {
        this->atlas = atlas;

        position = { 0.f, 0.f };
        speed    = { 0.f, 0.f };
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
        else                                                // The food element is falling
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
            // Draws the food element
            canvas.fill_rectangle
            (
                  position,
                { animation_slice->width, animation_slice->height  },
                  animation_slice,
                  Anchor::CENTER
            );
        }
    }


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool Food::contains_point(const Point2f &point)
    {
        float this_left = this->get_left_x ();

        if (point.coordinates.x () > this_left)
        {
            float this_bottom = this->get_bottom_y ();

            if (point.coordinates.y () > this_bottom)
            {
                float this_right = this_left + this->size.width;

                if (point.coordinates.x () < this_right)
                {
                    float this_top = this_bottom + this->size.height;

                    if (point.coordinates.y () < this_top)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

}
