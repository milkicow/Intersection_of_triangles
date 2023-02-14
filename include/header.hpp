#pragma once
#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>

#define LOX std::cerr << "ABOBA\n";
#undef  LOX
#define LOX

namespace Geo3D
{
const double epsilon = 10e-6;
enum Ort {x = 0b100, y = 0b010, z = 0b001};

inline bool is_equal(double arg1, double arg2 = 0) {
    if (abs(arg1 - arg2) < epsilon) return true;
    else return false;
}

inline int cmp_zero(double arg) {
    if (arg > epsilon) return 1;
    else if (arg < -epsilon) return -1;
    else return 0;
}
}
