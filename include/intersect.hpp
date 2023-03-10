#pragma once
#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include "line.hpp"
#include "segment.hpp"

namespace Geo3D
{

void compute_interval(const Triangle& triangle, const Vector& normal, double& min, double& max);
bool triangls_intersection_2D(const Triangle& t0, const Triangle& t1, Ort u, Ort v);

bool intersection(const Vector& point1, const Vector& point2);
bool intersection(const Vector& point, const Segment& segment);
bool intersection(const Vector& point, const Triangle& triangle);
bool intersection(const Segment& segment1, const Segment& segment2);
bool intersection(const Segment& segment, const Triangle& triangle);
bool triangles_intersection(const Triangle& triangle1, const Triangle& triangle2);
Vector point_of_intersection(Line line1, Line line2);
bool degenerate_intersection(const Triangle& triangle1, const Triangle& triangle2);
int receive_triangles();
int receive_triangles_without_octree();

}