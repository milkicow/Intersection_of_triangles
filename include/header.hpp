#pragma once
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

#define LOX std::cerr << "NORMALLY IN " << __FILE__ << " in " <<  __PRETTY_FUNCTION__ << " in LINE = " << __LINE__ << std::endl;
// #undef LOX
// #define LOX 

namespace Geo3D
{
const double epsilon = 10e-7;
enum Ort {x = 0, y = 1, z = 2};

inline bool is_equal(double arg1, double arg2 = 0) {
    if (std::abs(arg1 - arg2) < epsilon) return true;
    else return false;
}

inline int cmp_zero(double arg) {
    if (arg > epsilon) return 1;
    else if (arg < -epsilon) return -1;
    else return 0;
}
}
