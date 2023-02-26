#include "octree.hpp"

namespace Geo3D
{

Vector get_right_top_(const std::vector<Triangle>& triangles) {
    double max_coord_ = 0;

    for (int i = 0; i != triangles.size(); ++i) {
        if (triangles[i].v0_.max_coord() > triangles[i].v1_.max_coord() || is_equal(triangles[i].v0_.max_coord(), triangles[i].v1_.max_coord())
        &&  (triangles[i].v0_.max_coord() > triangles[i].v2_.max_coord() || is_equal(triangles[i].v0_.max_coord(), triangles[i].v2_.max_coord())))
            max_coord_ = triangles[i].v0_.max_coord();
        else if (triangles[i].v1_.max_coord() > triangles[i].v2_.max_coord() || is_equal(triangles[i].v1_.max_coord(), triangles[i].v2_.max_coord()))
            max_coord_ = triangles[i].v1_.max_coord();
        else 
            max_coord_ = triangles[i].v2_.max_coord();
    }

    return {max_coord_, max_coord_, max_coord_};
}

Vector get_left_bottom_(const std::vector<Triangle>& triangles) {
    double min_coord_ = 0;

    for (int i = 0; i != triangles.size(); ++i) {
        if (triangles[i].v0_.min_coord() < triangles[i].v1_.min_coord() || is_equal(triangles[i].v0_.min_coord(), triangles[i].v1_.min_coord())
        && (triangles[i].v0_.min_coord() < triangles[i].v2_.min_coord() || is_equal(triangles[i].v0_.min_coord(), triangles[i].v2_.min_coord())))
            min_coord_ = triangles[i].v0_.min_coord();
        else if (triangles[i].v1_.min_coord() < triangles[i].v2_.min_coord() || is_equal(triangles[i].v1_.min_coord(), triangles[i].v2_.min_coord()))
            min_coord_ = triangles[i].v1_.min_coord();
        else 
            min_coord_ = triangles[i].v2_.min_coord();
    }

    return {min_coord_, min_coord_, min_coord_};
}

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
    //parent->children_[octant] = std::move(std::make_unique<OctreeNode>(parent, left_bottom, right_top));
}


void split_triangles(std::unique_ptr<OctreeNode>& octree_node) {
    if (octree_node->triangles_.size() <= 2 || (octree_node->right_top_ - octree_node->left_bottom_).length() < 1) return;

    for (auto it = octree_node->triangles_.begin(); it != octree_node->triangles_.end(); ++it) {
        int octant = get_octant(octree_node->left_bottom_, octree_node->right_top_, *it);

        if (octant == -1) continue; 

        if (!octree_node->children_[octant]) create_octree_node(octree_node, octant);
        octree_node->children_[octant]->triangles_.push_back(*it);
        octree_node->triangles_.erase(it);
    }

    for (int i = 0; i != 8; ++i) {
        if (octree_node->children_[i]) split_triangles(octree_node->children_[i]);
    }
}

void Octree::fill_tree(const std::vector<Triangle>& triangles) {

    Triangle tmp;
    
    for(int i = 0; i != triangles.size(); ++i) {
        tmp = triangles[i];
        root_->triangles_.push_back(tmp);
    }

    left_bottom_max_ = get_left_bottom_(triangles);
    right_top_max_ = get_right_top_(triangles);

    split_triangles(root_);
}

}
