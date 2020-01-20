
// comentario

#include "Pancake.hpp"

using namespace basics;
using namespace std;

namespace example
{

    Pancake::Pancake(Atlas * atlas) : Food(atlas)
    {
        ascending_keyframes.push_back (ID(up_1));
        ascending_keyframes.push_back (ID(up_2));
        ascending_keyframes.push_back (ID(up_3));

        falling_keyframes.push_back (ID(down_1));
        falling_keyframes.push_back (ID(down_2));
        falling_keyframes.push_back (ID(down_3));
    }

}
