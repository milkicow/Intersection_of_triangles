#pragma once
#include "header.hpp"
#include "vector.hpp"

namespace Geo3D
{
struct Triangle final
{
public:
    enum Status { triangle = 0b100, segment = 0b010, point = 0b001 };
    Status status_;
    int number_;

    Vector v0_, v1_, v2_; // radius - vector 
    // int number; // number of the triangle that have intersection

    Triangle(const Vector& v0 = 0, const Vector& v1 = 0, const Vector& v2 = 0) : v0_(v0), v1_(v1), v2_(v2) {
        if (is_equal(v0, v1) && is_equal(v0, v2)) {
            // std::cout << "status = point\n";
            status_ = point;
        }
        else if (!is_equal(v0, v1) && !is_equal(v0, v2) && !is_equal(v1, v2) && !is_equal(cross(v1 - v0, v2 - v0), 0)) {
            // std::cout << "status = triangle\n";
            status_ = triangle;
        }
        else {
            // std::cout << "status = segment\n";
            status_ = segment;
        }
        //std::cout << v0_ << v1_ << v2_ << std::endl;
    };

    const Vector& operator[](int i) const {
        switch (i) {
            case 0: return v0_; break;
            case 1: return v1_; break;
            case 2: return v2_; break;
            default: std::cout << "incorrect number of verticle in triangle\n"; exit(EXIT_FAILURE);
        }
    }

    Vector& operator[](int i) {
        switch (i) {
            case 0: return v0_; break;
            case 1: return v1_; break;
            case 2: return v2_; break;
            default: std::cout << "incorrect number of verticle in triangle\n"; exit(EXIT_FAILURE);
        }
    }

    bool operator==(const Triangle& t) {
        return(t.v0_ == v0_ && t.v1_ == v1_ && t.v2_ == v2_);
    }

    void rotate_clockwise() { std::swap(v0_, v2_); std::swap(v2_, v1_); }

    double max_coord_abs() const { return std::max({v0_.max_coord_abs(), v1_.max_coord_abs(), v2_.max_coord_abs()}); }
};

std::ostream& operator<<(std::ostream& out, const Triangle& tr);
std::istream& operator>>(std::istream& in, Triangle& t);

}
