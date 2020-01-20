
// Comentario

#pragma once

#include "Food.hpp"

namespace example
{

	class Pancake : public Food
	{

		static const int number_of_ascending_keyframes = 3;
		static const int number_of_falling_keyframes   = 3;

	public:

		Pancake(Atlas * atlas);


		// set points pancakes
		int get_points () override {
			return 20;
		}
	};

}

