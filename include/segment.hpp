#pragma once
#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"

namespace Geo3D 
{

struct Segment final{
public:
    Vector v0_, v1_;

    Segment(const Vector& v0 = {0, 0, 0}, const Vector& v1 = {0, 0, 0}) : v0_(v0), v1_(v1) {};
    Segment(const Triangle& triangle) {
        Segment candidate1(triangle.v0_, triangle.v1_);
        Segment candidate2(triangle.v0_, triangle.v2_);
        Segment candidate3(triangle.v1_, triangle.v2_);

        if      (candidate1.point_belongs(triangle.v2_)) { v0_ = triangle.v0_; v1_ = triangle.v1_; }
        else if (candidate2.point_belongs(triangle.v1_)) { v0_ = triangle.v0_; v1_ = triangle.v2_; }
        else if (candidate3.point_belongs(triangle.v0_)) { v0_ = triangle.v1_; v1_ = triangle.v2_; }
    }

    bool point_belongs(const Vector& point) const {
        if((v1_ - v0_).normalized() != (point - v0_).normalized() && !is_equal(point, v0_)) {
            return false;
        }
        if (is_equal(v0_, v1_)) return is_equal(v0_, point);

        double t = (point - v0_).length() / (v1_ - v0_).length();

        if (t < 1 || is_equal(t, 1)) 
        {   
            return true;
        }
        else return false;
    }    
};

std::ostream& operator<<(std::ostream& out, const Segment& segment);

}