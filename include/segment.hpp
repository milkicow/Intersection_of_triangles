#include "header.hpp"
#include "vector.hpp"
#include "line.hpp"

namespace Geo3D 
{

struct Segment final{
public:
    Vector v0_, v1_;

    Segment(const Vector& v0, const Vector& v1) : v0_(v0), v1_(v1) {};
    Segment(const Triangle& triangle) {
        Segment candidate1(triangle.v0_, triangle.v1_);
        Segment candidate2(triangle.v0_, triangle.v2_);
        Segment candidate3(triangle.v1_, triangle.v2_);

        if      (candidate1.point_belongs(triangle.v2_)) *this = candidate1;
        else if (candidate2.point_belongs(triangle.v1_)) *this = candidate2;
        else if (candidate3.point_belongs(triangle.v0_)) *this = candidate3;
    }

    bool point_belongs(const Vector& point) const {
        if((v1_ - v0_).normalized() != (point - v0_).normalized() && !is_equal(point, v0_)) {
            //std::cout << "returned false\n";
            return false;
        }
        double t = (point - v0_).length() / (v1_ - v0_).length();
        //std::cout << "t = " << t << std::endl;
        if (t < 1 || is_equal(t, 1)) 
        {   
            //std::cout << "in if\n";
            return true;
        }
        else return false;
    }    
};

std::ostream& operator<<(std::ostream& out, const Segment& segment);

}