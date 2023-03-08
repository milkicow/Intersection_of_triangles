#include "line.hpp"

namespace Geo3D 
{

std::ostream& operator<<(std::ostream& out, const Line& line) {
    out << "direction = " << line.direction_ << "point = " << line.point_ << "\n";
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
    !(cmp_zero(dist.x_) != cmp_zero(dist.y_) && cmp_zero(dist.x_) != cmp_zero(dist.z_) && cmp_zero(dist.y_) != cmp_zero(dist.z_) && cmp_zero(dist.z_) == 1))
    {    
        t.rotate_clockwise();
        dist.rotate_clockwise();
    }
    Triangle t_projection = triangle_projection(t, int_line);

    Segment segment;
    segment.v0_ = t_projection[2] + (t_projection[1] - t_projection[2]) * std::abs(dist.z_) / (std::abs(dist.z_) + std::abs(dist.y_));
    segment.v1_ = t_projection[2] + (t_projection[0] - t_projection[2]) * std::abs(dist.z_) / (std::abs(dist.z_) + std::abs(dist.x_));
    
    return segment;
}

bool intersect_of_intervals(Segment segment1, Segment segment2) {
    return (segment1.point_belongs(segment2.v0_) || segment1.point_belongs(segment2.v1_) || 
    segment2.point_belongs(segment1.v0_) || segment2.point_belongs(segment1.v1_));
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
        t = 0;
        s = - d / a;
    }
    else {
        double invdet = 1 / det;
        s = (b * e - d * c) * invdet;
        t = (a * e - b * d) * invdet;
    }
    return (u + s * line1.direction_ - t * line2.direction_).length();
}

bool intersection(const Line& line1, const Line& line2) {
    return is_equal(distance_lines(line1, line2), 0);
}
}
