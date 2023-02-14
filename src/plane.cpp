#include "plane.hpp"

namespace Geo3D
{
std::ostream& operator<<(std::ostream& out, const Plane& plane) {
    out << plane.a_ << " " << plane.b_ << " " << plane.c_ << " " << plane.d_ << "\n";
    return out;
}

Line intersection_of_2_planes(Plane p1, Plane p2) // ! already get info about intersection of planes !
{   
    Line ret;
    ret.direction_ = cross(p1.normal(), p2.normal());

    double s1 = -p1.d_;
    double s2 = -p2.d_;

    double n1n2 =  p1.normal() * p2.normal();
    double n1sqr = p1.normal() * p1.normal();
    double n2sqr = p2.normal() * p2.normal();

    double a = (s2 * n1n2 - s1 * n2sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);
    double b = (s1 * n1n2 - s2 * n1sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);

    ret.point_ = a * p1.normal() + b * p2.normal();

    return ret;
}

Vector triangle_plane_dist(const Triangle& tr, const Plane& plane)
{   
    Vector ret;
    ret.x_ = (plane.a_ * tr.v0_.x_ + plane.b_ * tr.v0_.y_ + plane.c_ * tr.v0_.z_ + plane.d_) / plane.normal_length();
    ret.y_ = (plane.a_ * tr.v1_.x_ + plane.b_ * tr.v1_.y_ + plane.c_ * tr.v1_.z_ + plane.d_) / plane.normal_length();
    ret.z_ = (plane.a_ * tr.v2_.x_ + plane.b_ * tr.v2_.y_ + plane.c_ * tr.v2_.z_ + plane.d_) / plane.normal_length();

    return ret;
}

}
