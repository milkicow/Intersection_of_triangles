#pragma once
#include "header.hpp"
#include "vector.hpp"

namespace Geo3D
{
struct Triangle final
{
public:
    enum Status { triangle = 0b100, line = 0b010, point = 0b001 };
    Status status_;

    Vector v0_, v1_, v2_; // radius - vector 

    Triangle(const Vector& v0 = 0, const Vector& v1 = 0, const Vector& v2 = 0) : v0_(v0), v1_(v1), v2_(v2) {
        if      (!is_equal(v0, v1) && !is_equal(v0, v2) && !is_equal(v1, v2)) status_ = triangle;
        else if ((is_equal(v0, v1) && !is_equal(v0, v2)) || (is_equal(v0, v2) && !is_equal(v0, v1)) || (is_equal(v1, v2) && !is_equal(v1, v0))) status_ = line;
        else if (is_equal(v0, v1) && is_equal(v0, v2)) status_ = point;

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

    void rotate_clockwise() { std::swap(v0_, v1_); std::swap(v2_, v1_); }
};

std::ostream& operator<<(std::ostream& out, const Triangle& tr);

}
