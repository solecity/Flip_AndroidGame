/*
 * PANCAKE CLASS
 * Copyright © 2020+ Mariana Moreira
 */

#include "Pancake.hpp"

using namespace basics;
using namespace std;

namespace flip
{

    Pancake::Pancake(Atlas * atlas) : Food(atlas)
    {
        // Adds the corresponding sprites for the ascending movement
        ascending_keyframes.push_back (ID(up_1));
        ascending_keyframes.push_back (ID(up_2));
        ascending_keyframes.push_back (ID(up_3));

        // Adds the corresponding sprites for the falling movement
        falling_keyframes.push_back (ID(down_1));
        falling_keyframes.push_back (ID(down_2));
        falling_keyframes.push_back (ID(down_3));
    }

}
