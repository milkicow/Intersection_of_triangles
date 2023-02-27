#include "octree.hpp"

namespace Geo3D
{
    
int get_octant(const Vector& left_bottom, const Vector& right_top, const Triangle& triangle) {
    Vector center = (right_top + left_bottom) / 2 ;
    int octant[3]{}; // 3 field for each point of triangle

    for (int vertex = 0; vertex != 3; ++vertex) {
        for (int coord = 0; coord != 3; ++coord) {
            if (triangle[vertex][coord] > center[coord]) octant[vertex] |= (1 << coord);
            else if (is_equal(triangle[vertex][coord], center[coord])) return -1;
        }
        if (vertex && octant[vertex] != octant[vertex - 1]) return -1;
    }
    return octant[0];
}

void create_octree_node(std::unique_ptr<OctreeNode>& parent, int octant) {

    Vector center = (parent->left_bottom_ + parent->right_top_) / 2;
    Vector left_bottom, right_top;

    for (int coord = 0; coord != 3; ++coord) {
        if ((octant >> coord) & 1) {
            left_bottom[coord] = center[coord];
            right_top[coord] = parent->right_top_[coord];
        }
        else {
            left_bottom[coord] = parent->left_bottom_[coord];
            right_top[coord] = center[coord];
        }
    }
    parent->children_[octant] = std::make_unique<OctreeNode>(left_bottom, right_top);
}


void split_triangles(std::unique_ptr<OctreeNode>& octree_node) {
    if (octree_node->triangles_.size() <= 2 || (octree_node->right_top_ - octree_node->left_bottom_).length() < 1) return;

    auto it = octree_node->triangles_.begin();

    while (it != octree_node->triangles_.end()) {
        int octant = get_octant(octree_node->left_bottom_, octree_node->right_top_, *it);

        if (octant == -1) {
            ++it;
            continue; 
        }

        if (!octree_node->children_[octant]) {
            create_octree_node(octree_node, octant);
        }
        
        octree_node->children_[octant]->triangles_.push_back(*it);
        octree_node->triangles_.erase(it);        
    }

    for (int i = 0; i != 8; ++i) {
        if (octree_node->children_[i]) split_triangles(octree_node->children_[i]);
    }
}

void Octree::fill_tree(const std::vector<Triangle>& triangles) {

    Triangle tmp;
    double max_coord_abs = 0;

    for(int i = 0; i != triangles.size(); ++i) {
        tmp = triangles[i];
        root_->triangles_.push_back(tmp);

        if (tmp.max_coord_abs() > max_coord_abs) max_coord_abs = tmp.max_coord_abs();
    }

    Vector origin{0, 0, 0};
    root_->right_top_ = {max_coord_abs, max_coord_abs, max_coord_abs};
    root_->left_bottom_ =  origin - root_->right_top_;

    split_triangles(root_);
}

}
