#pragma once
#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"

namespace Geo3D
{
struct Line final // L(t) =  p + t*d
{
public:
    Vector point_;
    Vector direction_;

    Line(double direction = 0, Vector point = 0) : direction_(direction), point_(point) {};

};

std::ostream& operator<<(std::ostream& out, const Line& line);
Triangle triangle_projection(Triangle triangle, Line line);
std::vector<Vector> interval_on_line(Triangle t, Vector dist, Line int_line);
bool intersect_of_intervals(std::vector<Vector> segment1, std::vector<Vector> segment2);
}
