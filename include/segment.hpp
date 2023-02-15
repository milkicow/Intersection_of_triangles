#include "header.hpp"
#include "vector.hpp"
#include "line.hpp"

namespace Geo3D 
{

struct Segment final{
public:
    Vector v0_, v1_;

    Segment(const Vector& v0, const Vector& v1) : v0_(v0), v1_(v1) {};

    bool point_belongs(const Vector& point) const {
        if((v1_ - v0_).normalized() != (point - v0_).normalized()) return false;
        double t = (point - v0_).length() / (v1_ - v0_).length();
        if (t < 1 || is_equal(t, 1)) return true;
        else return false;
    }    
};

}