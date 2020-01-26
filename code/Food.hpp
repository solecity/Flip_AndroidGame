/*
 * FOOD CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#pragma once

#include <basics/Canvas>
#include <basics/Id>
#include <basics/Atlas>
#include <basics/Point>
#include <basics/Vector>

#include <vector>

using namespace basics;
using namespace std;

namespace flip
{

	class Food
	{

    protected:

        const float max_speed = 200.f;				///< Maximum speed of a food element

		Atlas * atlas;								///< Sprites atlas

		std::vector< Id > ascending_keyframes;		///< Id of ascending keyframes for the animation
		std::vector< Id > falling_keyframes;		///< Id of falling keyframes for the animation

		Point2f  position;							///< Position of the food element
		Vector2f speed;								///< Speed of the food element
		Size2f   size;                    			///< Sprite size (usually in virtual coordinates)

		int      anchor;                    		///< Indicates which point of the sprite will be placed in 'position' (x, y)
		float    radius;                    		///< Radius of the collider circle

	public:

		/**
         * Launches a new instance of Food
         * @param atlas Points to the sprites atlas. It must not be nullptr
         * @param radius Sets the radius of the collider
         */
	    Food(Atlas * atlas, float radius);

		/**
         * Destroys the food elements
         * @return the current position
         */
		virtual ~Food() = default;

		/**
         * This method gets the position of a food element
         * @return the current position
         */
		const Point2f & get_position() const
		{
			return position;
		}

		/**
         * This method gets the left position (in x) of an element
         * @return left x position
         */
		float get_left_x () const
		{
			return
					(anchor & 0x3) == basics::LEFT  ? position[0] :
					(anchor & 0x3) == basics::RIGHT ? position[0] - size[0] :
					 position[0] - size[0] * .5f;
		}

		/**
         * This method gets the right position (in x) of an element
         * @return right x position
         */
		float get_right_x () const
		{
			return get_left_x () + size.width;
		}

		/**
         * This method gets the bottom position (in y) of an element
         * @return bottom y position
         */
		float get_bottom_y () const
		{
			return
					(anchor & 0xC) == basics::BOTTOM ? position[1] :
					(anchor & 0xC) == basics::TOP    ? position[1] - size[1] :
					 position[1] - size[1] * .5f;
		}

		/**
         * This method gets the top position (in y) of an element
         * @return top y position
         */
		float get_top_y () const
		{
			return get_bottom_y () + size.height;
		}

		/**
         * This method gets the speed of a food element
         * @return the current speed
         */
		const Vector2f & get_speed() const
		{
			return speed;
		}

		/**
         * This method sets the new position of a food element
         * @param new_position
         */
		void set_position (const Point2f &new_position)
		{
			position = new_position;
		}

		/**
         * This method gets the points amount of a food element
         * @return the amount of points
         */
		virtual int get_points () {
			return 0;
		}

		/**
         * This method applies a force to the speed in y
         * @param force Force added to the current speed
         */
        void apply_force (float force)
        {
            speed.coordinates.y () += force;
        }

		/**
         * This method applies an impulse to the speed in y
         * @param impulse Impulse added to the current speed
         */
        void apply_impulse (float impulse)
        {
            speed.coordinates.y () = impulse;
        }

		/**
         * This method is automatically invoked once every frame so the scene can update its state
         */
		void update (float time);

		/**
         * This method is automatically invoked once every frame so the scene draws its content
         */
		void render (Canvas & canvas);

		/**
         * This method checks if the food element contains the given point
         * @param point Point that will be used to determine if a food element was clicked
         */
		bool contains_point (const Point2f & point);

	};
}
