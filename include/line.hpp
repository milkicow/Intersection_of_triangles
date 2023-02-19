#pragma once
#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"
#include "segment.hpp"

namespace Geo3D
{
struct Line final // L(t) =  p + t*d
{
public:
    Vector point_;
    Vector direction_;

    Line(const Vector& direction = 0, const Vector& point = 0) : direction_(direction), point_(point) {};

};

std::ostream& operator<<(std::ostream& out, const Line& line);
Triangle triangle_projection(Triangle triangle, Line line);
Segment interval_on_line(Triangle t, Vector dist, Line int_line);
bool intersect_of_intervals(Segment segment1, Segment segment2);
double distance_lines(const Line& line0, const Line& line1);
bool intersection(Line line1, Line line2);
}
