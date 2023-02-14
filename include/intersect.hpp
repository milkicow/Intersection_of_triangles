#pragma once
#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"
#include "plane.hpp"
#include "line.hpp"

namespace Geo3D
{

void compute_interval(const Triangle& triangle, const Vector& normal, double& min, double& max);
bool triangls_intersection_2D(Triangle t0, Triangle t1, Ort u, Ort v);
bool intersection_of_2_triangles(Triangle t1, Triangle t2);

}