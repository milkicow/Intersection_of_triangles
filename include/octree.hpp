#include "header.hpp"
#include "vector.hpp"
#include "triangle.hpp"
#include <vector>
#include <memory>

namespace Geo3D 
{


class OctreeNode final 
{
public:
    Vector left_bottom_, right_top_;

    std::vector<std::unique_ptr<OctreeNode>> children_;
    std::vector<Triangle> triangles_;

    OctreeNode(const Vector& left_bottom = {0, 0, 0}, const Vector& right_top = {0, 0, 0}) : 
    left_bottom_(left_bottom), right_top_(right_top), children_(8) {}

    void create_octree_node(int octant);
    void split_triangles();
    int intersections(std::vector<bool>& status);
    void intersections_with_children(const Triangle& triangle, int& intersection, std::vector<bool>& status);
    int get_octant(const Vector& left_bottom, const Vector& right_top, const Triangle& triangle);
};


class Octree final 
{
private:
    std::unique_ptr<OctreeNode> root_;

public:
    Octree() : root_(std::make_unique<OctreeNode>()) {}

    void fill_tree(const std::vector<Triangle>& triangles);
    int intersections(std::vector<bool>& status) { return root_->intersections(status); }
};
}
