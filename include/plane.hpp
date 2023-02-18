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
    Plane(const Triangle& tr) : Plane(tr.v1_ - tr.v0_, tr.v2_ - tr.v0_, tr.v0_) {};
    Plane(const Vector& v1, const Vector& v2, const Vector& point) {
        a_ = v1.y_ * v2.z_ - v2.y_ * v1.z_;
        b_ = v1.z_ * v2.x_ - v2.z_ * v1.x_;
        c_ = v1.x_ * v2.y_ - v2.x_ * v1.y_;
        d_ = - (a_ * point.x_ + b_ * point.y_ + c_ * point.z_);
    }

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
