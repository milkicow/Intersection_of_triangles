#pragma once
#include "header.hpp"
#include "triangle.hpp"
#include "vector.hpp"
#include "line.hpp"

namespace Geo3D 
{
struct Plane final
{
public:
    double a_, b_, c_, d_;

    Plane(double a = 0, double b = 0, double c = 0, double d = 0) : a_(a), b_(b), c_(c), d_(d) {};
    Plane(const Triangle& tr) {
        Vector v1(tr.v1_ - tr.v0_), v2(tr.v2_ - tr.v0_);
        //std::cout << v1 << v2;

        a_ = v1.y_ * v2.z_ - v2.y_ * v1.z_;
        b_ = v1.z_ * v2.x_ - v2.z_ * v1.x_;
        c_ = v1.x_ * v2.y_ - v2.x_ * v1.y_;
        d_ = - (a_ * tr.v0_.x_ + b_ * tr.v0_.y_ + c_ * tr.v0_.z_);
    };

    Vector normal() {
        return Vector(this -> a_, this -> b_, this -> c_);
    }

    double normal_length() const {
        return sqrt(this->a_ * this->a_ + this->b_ * this->b_ + this->c_ * this->c_);
    }

    Plane normalized() {
        return Plane(this->a_ / this->normal_length(), this->b_ / this->normal_length(), this->c_ / this->normal_length(), this->d_ / this->normal_length());
    }
};

std::ostream& operator<<(std::ostream& out, const Plane& plane);
Vector triangle_plane_dist(const Triangle& tr, const Plane& plane);
double point_plane_dist(const Vector& vec, const Plane& plane);
Line intersection_of_2_planes(Plane p1, Plane p2);

}
