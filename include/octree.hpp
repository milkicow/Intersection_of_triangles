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

    //std::unique_ptr<OctreeNode> parent_;
    std::vector<std::unique_ptr<OctreeNode>> children_;
    std::vector<Triangle> triangles_;

    OctreeNode(const Vector& left_bottom = {0, 0, 0}, const Vector& right_top = {0, 0, 0}) : 
    left_bottom_(left_bottom), right_top_(right_top), children_(8) {}

    // Vector& get_left_bottom_() { return left_bottom_; }
    // Vector& get_right_top_()   { return right_top_; }

};


class Octree final 
{
private:
    std::unique_ptr<OctreeNode> root_;

public:
    Octree() : root_(std::make_unique<OctreeNode>()) {}

    // Vector& get_left_bottom_() { return left_bottom_max_; }
    // Vector& get_right_top_()   { return right_top_max_; }
    void fill_tree(const std::vector<Triangle>& triangles);
    std::unique_ptr<OctreeNode>& get_root_() { return root_; }

};

// Vector get_right_top_(const std::vector<Triangle>& triangles);
// Vector get_left_bottom_(const std::vector<Triangle>& triangles);
int get_octant(const Vector& left_bottom, const Vector& right_top, const Triangle& triangle);
void create_octree_node(const OctreeNode& parent, int octant);
void split_triangles(std::unique_ptr<OctreeNode>& octree_node);

}
