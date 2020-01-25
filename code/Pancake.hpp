/*
 * PANCAKE CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#pragma once

#include "Food.hpp"

namespace flip
{

	class Pancake : public Food
	{

		static const int number_of_ascending_keyframes = 3;			///< Number of sprites with the ascending movement
		static const int number_of_falling_keyframes   = 3;			///< Number of sprites with the falling movement

	public:

		/**
         * Launches a new instance of Pancake
         * @param atlas  -  Points to the sprites atlas. It must not be nullptr
         */
		Pancake (Atlas * atlas);

		/**
         * Sets how many points a pancake is worth
         */
		int get_points () override {
			return 20;
		}
	};

}

