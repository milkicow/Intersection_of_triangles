#include "plane.hpp"

namespace Geo3D
{
std::ostream& operator<<(std::ostream& out, const Plane& plane) {
    out << plane.a_ << " " << plane.b_ << " " << plane.c_ << " " << plane.d_ << "\n";
    return out;
}

Line intersection_of_2_planes(Plane p1, Plane p2) {// ! already get info about intersection of planes !
   
    Line tmp;
    tmp.direction_ = cross(p1.normal(), p2.normal());

    double s1 = -p1.d_;
    double s2 = -p2.d_;

    double n1n2 =  p1.normal() * p2.normal();
    double n1sqr = p1.normal() * p1.normal();
    double n2sqr = p2.normal() * p2.normal();

    double a = (s2 * n1n2 - s1 * n2sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);
    double b = (s1 * n1n2 - s2 * n1sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);

    tmp.point_ = a * p1.normal() + b * p2.normal();

    return tmp;
}

double point_plane_dist(const Vector& vec, const Plane& plane) {
    return (plane.a_ * vec.x_ + plane.b_ * vec.y_ + plane.c_ * vec.z_ + plane.d_);
}

Vector triangle_plane_dist(const Triangle& tr, const Plane& plane) {   
    //std::cout << "triangle = " << tr;
    //std::cout << "plane = " << plane;
    Vector tmp;
    tmp.x_ = point_plane_dist(tr.v0_, plane);
    tmp.y_ = point_plane_dist(tr.v1_, plane);
    tmp.z_ = point_plane_dist(tr.v2_, plane);

    return tmp;
}

}
