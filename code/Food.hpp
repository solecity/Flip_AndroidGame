
// Comentario

#pragma once

#include <basics/Canvas>
#include <basics/Id>
#include <basics/Atlas>
#include <basics/Point>
#include <basics/Vector>
#include <vector>

using namespace basics;
using namespace std;

namespace example
{

	class Food
	{
    protected:

        const float max_speed = 200.f;

		Atlas * atlas;

		std::vector< Id > ascending_keyframes;
		std::vector< Id > falling_keyframes;

		Point2f  position;
        float   scale;
		Vector2f speed;

	public:

	    Food(Atlas * atlas);

		virtual ~Food() = default;

		const Point2f & get_position() const
		{
			return position;
		}

		const Vector2f & get_speed() const
		{
			return speed;
		}

        void set_scale (float &new_scale)
        {
            scale = new_scale;
        }

		void set_position (const Point2f &new_position)
		{
			position = new_position;
		}

		virtual int get_points () {
			return 0;
		}

        void apply_force (float force)
        {
            speed.coordinates.y () += force;
        }

        void apply_impulse (float impulse)
        {
            speed.coordinates.y () = impulse;
        }

		void update (float time);

		void render (Canvas & canvas);

		bool contains_point (const Point2f & point);

	};

}
