#include "vector.hpp"

namespace Geo3D
{
std::ostream& operator<<(std::ostream& out, const Vector& vector) {
    out << vector.x_ << " " << vector.y_ << " " << vector.z_ << "\n";
    return out;
}

bool is_equal(const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs - rhs;
    if (is_equal(tmp.x_) && is_equal(tmp.y_) && is_equal(tmp.z_)) return true;
    else return false;
}

Vector operator+(const Vector& lhs, const Vector& rhs) { Vector tmp(lhs); tmp += rhs; return tmp; }

Vector operator-(const Vector& lhs, const Vector& rhs) { Vector tmp(lhs); tmp -= rhs; return tmp; }

Vector operator*(double num, const Vector& vec) { return Vector(num * vec.x_, num * vec.y_, num * vec.z_); }

Vector operator*(const Vector& vec, double num) { return Vector(num * vec.x_, num * vec.y_, num * vec.z_); }

Vector operator/(const Vector& vec, double num) { return Vector(vec.x_ / num, vec.y_ / num, vec.z_ / num); }

bool operator==(const Vector& lhs, const Vector& rhs) {
    return (is_equal(lhs.x_, rhs.x_) && is_equal(lhs.y_, rhs.y_) && is_equal(lhs.z_, rhs.z_));
}

Vector Vector::projection_on(const Vector& vec) const {
    return (*this * vec * vec) / (vec * vec);
}

Vector cross(const Vector& lhs, const Vector& rhs) {
    return Vector(lhs.y_ * rhs.z_ - rhs.y_ * lhs.z_, rhs.x_ * lhs.z_ - lhs.x_ * rhs.z_, lhs.x_ * rhs.y_ - rhs.x_ * lhs.y_);
}

double det(const Vector& v0, const Vector& v1, const Vector& v2)
{
    return v0.x_ * v1.y_ * v2.z_ - v0.x_ * v2.y_ * v1.z_ - v0.y_ * v1.x_ * v2.z_ + v0.y_ * v2.x_ * v1.z_ + v0.z_ * v1.x_ * v2.y_ - v0.z_ * v2.x_ * v1.y_;
}

Vector perpendicular(const Vector& side, Ort u, Ort v) { // maybe -> abs(normal) = 1 ?!
    Vector tmp;
    tmp[x] = side[v];
    tmp[y] = -side[u];
    return tmp; 
}

Ort max_ort_component(const Vector& vec) {
    double max = std::max({abs(vec.x_), abs(vec.y_), abs(vec.z_)});

    if (is_equal(max, abs(vec.x_))) return Ort::x;
    else if (is_equal(max, abs(vec.y_))) return Ort::y;
    else return Ort::z;
}

std::istream& operator>>(std::istream& in, Vector& vec) {
    return in >> vec.x_ >> vec.y_ >> vec.z_;
}

}

