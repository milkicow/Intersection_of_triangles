#include "line.hpp"

namespace Geo3D 
{

std::ostream& operator<<(std::ostream& out, const Line& line) {
    out << line.point_ << line.direction_ << "\n";
    return out;
}

Triangle triangle_projection(Triangle triangle, Line line) {
    Triangle tmp;

    for (int i = 0; i != 3; ++i) {
        tmp[i] = line.point_ + triangle[i].projection_on(line.direction_);
    }   
    return tmp;
} 

Segment interval_on_line(Triangle t, Vector dist, Line int_line) {

    while(!(cmp_zero(dist.x_) == cmp_zero(dist.y_) && cmp_zero(dist.x_) != cmp_zero(dist.z_)) &&
    (cmp_zero(dist.x_) != cmp_zero(dist.y_) && cmp_zero(dist.x_) != cmp_zero(dist.z_) && cmp_zero(dist.y_) != cmp_zero(dist.z_) && cmp_zero(dist.z_) != 1))
    {   
        std::cout << cmp_zero(dist.x_) << cmp_zero(dist.y_) << cmp_zero(dist.z_) << std::endl;
        LOX
        t.rotate_clockwise();
        dist.rotate_clockwise();
        // std::cout << "rotated clockwise\n";
        // std::cout << "triangle:\n" << t;
        // std::cout << "distance:\n" << dist;
    }

    LOX
    Triangle t_projection = triangle_projection(t, int_line);

    //std::cout << "triangle:\n" << t;
    //std::cout << "triangle projection:\n";
    //std::cout << t_projection;

    Segment segment;
    LOX
    segment.v0_  = t_projection[0] + (t_projection[2] - t_projection[0]).length() * abs(dist.x_) / (abs(dist.x_) + abs(dist.z_)) * int_line.direction_.normalized();
    segment.v1_  = t_projection[1] + (t_projection[2] - t_projection[1]).length() * abs(dist.y_) / (abs(dist.y_) + abs(dist.z_)) * int_line.direction_.normalized();

    return segment;
}

bool intersect_of_intervals(Segment segment1, Segment segment2) {

    //std::cout << segment2[1] << segment1[1];
    Vector v00 = segment2.v0_ - segment1.v0_;
    Vector v01 = segment2.v0_ - segment1.v1_;

    //std::cout << "v00 = " << v00 << "v01 = " << v01 ;
    if (cmp_zero(v00 * v01) == -1 || cmp_zero(v00 * v01) == 0) return true;

    Vector v10 = segment2.v1_ - segment1.v0_;
    Vector v11 = segment2.v1_ - segment1.v1_;

    if (cmp_zero(v10 * v11) == -1 || cmp_zero(v10 * v11) == 0) return true;
    else return false;
}

double distance_lines(const Line& line1, const Line& line2) {

    Vector u = line1.point_ - line2.point_;
    double a = line1.direction_ * line1.direction_;
    double b = line1.direction_ * line2.direction_;
    double c = line2.direction_ * line2.direction_;
    double d = line1.direction_ * u;
    double e = line2.direction_ * u;

    double det = a * c - b * b;
    double t, s;

    if(is_equal(det, 0)) {
        s = 0;
        if (b > c) t = d / b;
        else       t = e / c;
    }
    else {
        double invdet = 1 / det;
        s = (b * e - d * c) * invdet;
        t = (a * e - b * d) * invdet;
    }

    return (u + s * line1.direction_ + t * line2.direction_).length();
}

bool intersection(Line line1, Line line2) {
    return is_equal(distance_lines(line1, line2), 0);
}
}
