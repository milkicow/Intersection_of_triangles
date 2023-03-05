#include "octree.hpp"
#include "intersect.hpp"

namespace Geo3D
{
    
int OctreeNode::get_octant(const Vector& left_bottom, const Vector& right_top, const Triangle& triangle) {
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

void OctreeNode::create_octree_node(int octant) {

    Vector center = (left_bottom_ + right_top_) / 2;
    Vector child_left_bottom, child_right_top;

    for (int coord = 0; coord != 3; ++coord) {
        if ((octant >> coord) & 1) {
            child_left_bottom[coord] = center[coord];
            child_right_top[coord] = right_top_[coord];
        }
        else {
            child_left_bottom[coord] = left_bottom_[coord];
            child_right_top[coord] = center[coord];
        }
    }
    children_[octant] = std::make_unique<OctreeNode>(child_left_bottom, child_right_top);
}


void OctreeNode::split_triangles() {
    if (triangles_.size() <= 2 || (right_top_ - left_bottom_).length() < 1) return;

    auto it = triangles_.begin();

    while (it != triangles_.end()) {
        int octant = get_octant(left_bottom_, right_top_, *it);

        if (octant == -1) {
            ++it;
            continue; 
        }

        if (!children_[octant]) {
            create_octree_node(octant);
        }
        
        children_[octant]->triangles_.push_back(*it);
        triangles_.erase(it);        
    }

    for (int i = 0; i != 8; ++i) {
        if (children_[i]) children_[i]->split_triangles();
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

    root_->right_top_ = {max_coord_abs, max_coord_abs, max_coord_abs};
    root_->left_bottom_ = {-max_coord_abs, -max_coord_abs, -max_coord_abs};

    root_->split_triangles();
}

int OctreeNode::intersections(std::vector<bool>& status) {

    int intersection = 0;

    for (auto first_it = triangles_.begin(); first_it != triangles_.end(); ++first_it) {
        for (auto second_it = first_it + 1; second_it != triangles_.end(); ++second_it) {
            if (triangles_intersection(*first_it, *second_it)) {
                ++intersection;
                status[first_it->number_] = true;
                status[second_it->number_] = true;
            }
        }
        intersections_with_children(*first_it, intersection, status);
    }

    for (int child = 0; child != 8; ++child) {
        if (children_[child] == nullptr) continue;
        intersection += children_[child]->intersections(status);
    }
    return intersection;
}

void OctreeNode::intersections_with_children(const Triangle& triangle, int& intersection, std::vector<bool>& status) {

    for (int child = 0; child != 8; ++child) {
        if (children_[child] == nullptr) continue;

        for (auto it = children_[child]->triangles_.begin(); it != children_[child]->triangles_.end(); ++it) {
            if (triangles_intersection(triangle, *it)) {
                ++intersection;
                status[triangle.number_] = true;
                status[it->number_] = true;
            }
        }
        children_[child]->intersections_with_children(triangle, intersection, status);
    }
}

}
