#pragma once 
#include "header.hpp"

namespace Geo3D
{
struct Vector final
{
public:
    double x_, y_, z_;

    Vector(double x = 0, double y = 0, double z = 0) : x_(x), y_(y), z_(z) {}; 

    Vector& operator-() {
        x_ = -x_; y_ = -y_; z_ = -z_;
        return *this;
    }

    Vector& operator+=(const Vector& rhs) {
        x_ += rhs.x_; y_ += rhs.y_; z_ += rhs.z_;
        return *this;
    }

    Vector& operator-=(const Vector& rhs) {
        x_ -= rhs.x_; y_ -= rhs.y_; z_ -= rhs.z_;
        return *this;
    }

    double operator*(const Vector& rhs) const {
        return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
    }

    double length() const {
        return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    Vector normalized() const {
        return Vector(x_ / this->length() , y_ / this->length(), z_ / this->length());
    }

    Vector projection_on(const Vector& vec) const;

    double& operator[](int ort) {
        switch (ort) {
            case Ort::x: return this->x_; break;
            case Ort::y: return this->y_; break;
            case Ort::z: return this->z_; break;
            default: throw std::invalid_argument("invalid_argument = " + std::to_string(ort));
        }
    }

    const double& operator[](int ort) const {
        switch (ort) {
            case Ort::x: return this->x_; break;
            case Ort::y: return this->y_; break;
            case Ort::z: return this->z_; break;
            default: throw std::invalid_argument("invalid_argument = " + std::to_string(ort));
        }
    }

    void rotate_clockwise() {std::swap(y_, z_); std::swap(x_, y_);};

    double max_coord_abs() const { return std::max({abs(x_), abs(y_), abs(z_)}); }
    // double min_coord() const { return std::min({x_, y_, z_}); }
};

std::ostream& operator<<(std::ostream& out, const Vector& vector);
std::istream& operator>>(std::istream& in, Vector& vec);

bool is_equal(const Vector& lhs, const Vector& rhs);
Vector operator+(const Vector& lhs, const Vector& rhs);
Vector operator-(const Vector& lhs, const Vector& rhs);
Vector operator*(double num, const Vector& vec);
Vector operator*(const Vector& vec, double num);
Vector operator/(const Vector& vec, double num);
bool operator==(const Vector& lhs, const Vector& rhs);
Vector cross(const Vector& lhs, const Vector& rhs);
double det(const Vector& v0, const Vector& v1, const Vector& v2);
Vector perpendicular(const Vector& side, Ort u, Ort v);
Ort max_ort_component(const Vector& vec);
}
