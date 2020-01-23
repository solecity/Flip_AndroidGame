/*
 * PANCAKE CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#pragma once

#include "Food.hpp"

namespace flip
{

	class Strawberry : public Food
	{

		static const int number_of_ascending_keyframes = 3;			///< Number of sprites with the ascending movement
		static const int number_of_falling_keyframes   = 3;			///< Number of sprites with the falling movement

	public:

		Strawberry(Atlas * atlas);

		/**
         * Sets how many points a pancake is worth
         */
		int get_points () override {
			return 40;
		}
	};

}

