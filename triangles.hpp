#include <iostream>
#include <assert.h>
#include <math.h>
#include <vector>

#define LOX std::cerr << "ABOBA\n";
#undef  LOX
#define LOX

namespace Geo3D
{

const double epsilon = 10e-6;
enum Ort {x = 0b100, y = 0b010, z = 0b001};

bool is_equal(double arg1, double arg2 = 0) {
    if (abs(arg1 - arg2) < epsilon) return true;
    else return false;
}

int cmp_zero(double arg) {
    if (arg > epsilon) return 1;
    else if (arg < -epsilon) return -1;
    else return 0;
}

struct Vector;
Vector operator*(double num, const Vector& vec);
Vector operator/(const Vector& vec, double num);

struct Vector final
{
public:
    double x_, y_, z_;

    Vector(double x = 0, double y = 0, double z = 0) : x_(x), y_(y), z_(z) {}; 

    Vector& operator+=(const Vector& rhs) {
        x_ += rhs.x_; y_ += rhs.y_; z_ = rhs.z_;
        return *this;
    }

    Vector& operator-=(const Vector& rhs) {
        x_ -= rhs.x_; y_ -= rhs.y_; z_ -= rhs.z_;
        return *this;
    }

    double operator*(const Vector& rhs) const {
        return this -> x_ * rhs.x_ + this -> y_ * rhs.y_ + this -> z_ * rhs.z_;
    }

    double length() const {
        return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    Vector normalized() const {
        return Vector(x_ / this->length() , y_ / this->length(), z_ / this->length());
    }

    Vector projection_on(const Vector& vec) const {
        return (*this * vec * vec) / (vec * vec);
    }

    double& operator[](Ort ort) {
        switch (ort) {
            case Ort::x: return this->x_; break;
            case Ort::y: return this->y_; break;
            case Ort::z: return this->z_; break;
            default: std::cout << "incorrect number of coordinate in Vector\n"; exit(EXIT_FAILURE);
        }
    }

    const double& operator[](Ort ort) const{
        switch (ort) {
            case Ort::x: return this->x_; break;
            case Ort::y: return this->y_; break;
            case Ort::z: return this->z_; break;
            default: std::cout << "incorrect number of coordinate in Vector\n"; exit(EXIT_FAILURE);
        }
    }

    void rotate_clockwise() {std::swap(y_, z_); std::swap(x_, y_);};
};

std::ostream& operator<<(std::ostream& out, const Vector& vector) {
    out << vector.x_ << " " << vector.y_ << " " << vector.z_ << "\n";
    return out;
}

Vector operator+(const Vector& lhs, const Vector& rhs) { Vector tmp(lhs); tmp += rhs; return tmp; }

Vector operator-(const Vector& lhs, const Vector& rhs) { Vector tmp(lhs); tmp -= rhs; return tmp; }

Vector operator*(double num, const Vector& vec) { return Vector(num * vec.x_, num * vec.y_, num * vec.z_); }

Vector operator*(const Vector& vec, double num) { return Vector(num * vec.x_, num * vec.y_, num * vec.z_); }

Vector operator/(const Vector& vec, double num) { return Vector(vec.x_ / num, vec.y_ / num, vec.z_ / num); }

bool operator==(const Vector& lhs, const Vector& rhs) {
    return (is_equal(lhs.x_, rhs.x_) && is_equal(lhs.y_, rhs.y_) && is_equal(lhs.z_, rhs.z_));
}

bool is_equal(const Vector& lhs, const Vector& rhs) {
    Vector tmp = lhs - rhs;
    if (is_equal(tmp.x_) && is_equal(tmp.y_) && is_equal(tmp.z_)) return true;
    else return false;
}

struct Triangle final
{
private:
    enum Status { triangle = 0b100, line = 0b010, point = 0b001 };
    Status status;

public:
    Vector v0_, v1_, v2_; // radius - vector 

    Triangle(Vector v0 = 0, Vector v1 = 0, Vector v2 = 0) : v0_(v0), v1_(v1), v2_(v2) {
        if      (!is_equal(v0, v1) && !is_equal(v0, v2) && !is_equal(v1, v2)) status = triangle;
        else if ((is_equal(v0, v1) && !is_equal(v0, v2)) || (is_equal(v0, v2) && !is_equal(v0, v1)) || (is_equal(v1, v2) && !is_equal(v1, v0))) status = line;
        else if (is_equal(v0, v1) && is_equal(v0, v2)) status = point;

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

    void rotate_clockwise() { std::swap(v0_, v1_); std::swap(v2_, v1_); }
};

std::ostream& operator<<(std::ostream& out, const Triangle& tr) {
    out << tr.v0_.x_ << " " << tr.v0_.y_ << " " << tr.v0_.z_ << " " << tr.v1_.x_ << " " << tr.v1_.y_ << " " << tr.v1_.z_ << " " << tr.v2_.x_ << " " << tr.v2_.y_ << " " << tr.v2_.z_ << "\n";
    return out;
}

struct Plane final
{
public:
    double a_, b_, c_, d_;

    Plane(double a = 0, double b = 0, double c = 0, double d = 0) : a_(a), b_(b), c_(c), d_(d) {};
    Plane(Triangle tr) {
        Vector v1(tr.v1_ - tr.v0_), v2(tr.v2_ - tr.v0_);
        //std::cout << v1 << v2;

        a_ = v1.y_ * v2.z_ - v2.y_ * v1.z_;
        b_ = v1.z_ * v2.x_ - v2.z_ * v1.x_;
        c_ = v1.x_ * v2.y_ - v2.x_ * v1.y_;
        d_ = - (a_ * tr.v0_.x_ + b_ * tr.v0_.y_ + c_ * tr.v0_.z_);
    };

    Vector normal() {
        return Vector(this -> a_, this -> b_, this -> c_);
    }

    double normal_length() const {
        return sqrt(this->a_ * this->a_ + this->b_ * this->b_ + this->c_ * this->c_);
    }

    Plane normalized() {
        return Plane(this->a_ / this->normal_length(), this->b_ / this->normal_length(), this->c_ / this->normal_length(), this->d_ / this->normal_length());
    }
};

std::ostream& operator<<(std::ostream& out, const Plane& plane) {
    out << plane.a_ << " " << plane.b_ << " " << plane.c_ << " " << plane.d_ << "\n";
    return out;
}

Vector cross(const Vector& lhs, const Vector& rhs) {
    return Vector(lhs.y_ * rhs.z_ - rhs.y_ * lhs.z_, rhs.x_ * lhs.z_ - lhs.x_ * rhs.z_, lhs.x_ * rhs.y_ - rhs.x_ * lhs.y_);
}

struct Line final // L(t) =  p + t*d
{
public:
    Vector point_;
    Vector direction_;

    Line(double direction = 0, Vector point = 0) : direction_(direction), point_(point) {};

};

std::ostream& operator<<(std::ostream& out, const Line& line) {
    out << line.point_ << line.direction_ << "\n";
    return out;
}

double det(const Vector& v0, const Vector& v1, const Vector& v2)
{
    return v0.x_ * v1.y_ * v2.z_ - v0.x_ * v2.y_ * v1.z_ - v0.y_ * v1.x_ * v2.z_ + v0.y_ * v2.x_ * v1.z_ + v0.z_ * v1.x_ * v2.y_ - v0.z_ * v2.x_ * v1.y_;
}

Vector triangle_plane_dist(const Triangle& tr, const Plane& plane)
{   
    Vector ret;
    ret.x_ = (plane.a_ * tr.v0_.x_ + plane.b_ * tr.v0_.y_ + plane.c_ * tr.v0_.z_ + plane.d_) / plane.normal_length();
    ret.y_ = (plane.a_ * tr.v1_.x_ + plane.b_ * tr.v1_.y_ + plane.c_ * tr.v1_.z_ + plane.d_) / plane.normal_length();
    ret.z_ = (plane.a_ * tr.v2_.x_ + plane.b_ * tr.v2_.y_ + plane.c_ * tr.v2_.z_ + plane.d_) / plane.normal_length();

    return ret;
}

Line intersection_of_2_planes(Plane p1, Plane p2) // ! already get info about intersection of planes !
{   
    Line ret;
    ret.direction_ = cross(p1.normal(), p2.normal());

    double s1 = -p1.d_;
    double s2 = -p2.d_;

    double n1n2 =  p1.normal() * p2.normal();
    double n1sqr = p1.normal() * p1.normal();
    double n2sqr = p2.normal() * p2.normal();

    double a = (s2 * n1n2 - s1 * n2sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);
    double b = (s1 * n1n2 - s2 * n1sqr) / (n1n2 * n1n2 - n1sqr * n2sqr);

    ret.point_ = a * p1.normal() + b * p2.normal();

    return ret;
}

Ort max_ort_component(Vector vec) {
    double max = std::max({abs(vec.x_), abs(vec.y_), abs(vec.z_)});

    if (is_equal(max, abs(vec.x_))) return Ort::x;
    else if (is_equal(max, abs(vec.y_))) return Ort::y;
    else if (is_equal(max, abs(vec.z_))) return Ort::z;
    else std::cout << "can't define max\n"; exit(EXIT_FAILURE);

}

Vector perpendicular(Vector side, Ort u, Ort v) { // maybe -> abs(normal) = 1 ?!
    Vector tmp;
    tmp[x] = side[v];
    tmp[y] = -side[u];
    return tmp; 
}

void compute_interval(const Triangle& triangle, const Vector& normal, double& min, double& max) {
    min = max = normal * triangle[0];

    for (int i = 1; i < 3; ++i) {
        double value = normal * triangle[i];
        if      (value < min) min = value;
        else if (value > max) max = value;
    }
}

bool triangls_intersection_2D(Triangle t0, Triangle t1, Ort u, Ort v) {

    double min0, max0, min1, max1;

    for (int i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++) {
        Vector normal = perpendicular(t0[i0] - t0[i1], u, v);

        compute_interval(t0, normal, min0, max0);
        compute_interval(t1, normal, min1, max1);

        if (max1 < min0 || max0 < min1) return false;
    }

    for(int i0 = 0, i1 = 2; i0 < 3; i1 = i0, i0++) {
        Vector normal = perpendicular(t1[i0] - t1[i1], u, v);

        compute_interval(t0, normal, min0, max0);
        compute_interval(t1, normal, min1, max1);

        if (max1 < min0 || max0 < min1) return false;
    }
    return true;
}

Triangle triangle_projection(Triangle triangle, Line line) {
    Triangle tmp;

    for (int i = 0; i != 3; ++i) {
        tmp[i] = line.point_ + triangle[i].projection_on(line.direction_);
    }   
    return tmp;
} 

std::vector<Vector> interval_on_line(Triangle t, Vector dist, Line int_line) {

    while(!(cmp_zero(dist.x_) == cmp_zero(dist.y_) && cmp_zero(dist.x_) != cmp_zero(dist.z_)))
    {   
        t.rotate_clockwise();
        dist.rotate_clockwise();
        //std::cout << "rotated clockwise\n";
        //std::cout << "triangle:\n" << t;
        //std::cout << "distance:\n" << dist;
    }

    LOX
    Triangle t_projection = triangle_projection(t, int_line);

    //std::cout << "triangle:\n" << t;
    //std::cout << "triangle projection:\n";
    //std::cout << t_projection;
    std::vector<Vector> segment{};
    LOX
    Vector v0 = t_projection[0] + (t_projection[2] - t_projection[0]).length() * (dist.x_) / (dist.x_ - dist.z_) * int_line.direction_.normalized();
    Vector v1 = t_projection[1] + (t_projection[2] - t_projection[1]).length() * (dist.y_) / (dist.y_ - dist.z_) * int_line.direction_.normalized();

    //std::cout << "v0 = " << v0;
    //std::cout << "v1 = " << v1;
    segment.push_back(v0);
    segment.push_back(v1);

    return segment;
}

bool intersect_of_intervals(std::vector<Vector> segment1, std::vector<Vector> segment2) {

    //std::cout << segment2[1] << segment1[1];
    Vector v00 = segment2[0] - segment1[0];
    Vector v01 = segment2[1] - segment1[0];

    //std::cout << v01;
    if (cmp_zero(v00 * v01) == -1 || cmp_zero(v00 * v01) == 0) return true;

    Vector v10 = segment2[0] - segment1[1];
    Vector v11 = segment2[1] - segment1[1];

    if (cmp_zero(v10 * v11) == -1 || cmp_zero(v10 * v11) == 0) return true;
    else return false;
}

bool intersection_of_2_triangles(Triangle t1, Triangle t2) {

// 1) DO ! : check t1 and t2 on degeneracy !

// 2) :
    Plane plane1(t1);
    //std::cout << plane1;

// 3) :
    Vector dist2 = triangle_plane_dist(t2, plane1);
    //std::cout << dist2;
// 4) :
    int dist2_sign = cmp_zero(dist2.x_) + cmp_zero(dist2.y_) + cmp_zero(dist2.z_);
    if (dist2_sign == 3 || dist2_sign == -3) return false;
    //std::cout << dist2_sign << std::endl;
// 5) :
    Plane plane2(t2);
    //std::cout << plane2;
// 6) :
    plane1.normalized();
    plane2.normalized();

    //std::cout << plane1 << plane2;

    if (plane1.normal() == plane2.normal()) {
        if (plane1.d_ != plane2.d_) return false;
        else {
            std::cout << "plane1.normal() == plane2.normal()" << std::endl;

            Ort ort = max_ort_component(plane1.normal());
            
            Ort u, v;
            switch(ort)
            {
                case Ort::x: u = Ort::y; v = Ort::z; break;
                case Ort::y: u = Ort::x; v = Ort::z; break;
                case Ort::z: u = Ort::x; v = Ort::y; break;
                default: std::cout << "Can't define new orts\n"; exit(EXIT_FAILURE);
            }

            return triangls_intersection_2D(t1, t2, u, v);
        }
    }

// 7) :
    Vector dist1 = triangle_plane_dist(t1, plane2);

    //std::cout << dist1;

    int dist1_sign = cmp_zero(dist1.x_) + cmp_zero(dist1.y_) + cmp_zero(dist1.z_);
    if (dist1_sign == 3 || dist1_sign == -3) return false;

    //std::cout << dist1_sign << std::endl;

// 8) :
    Line int_line = intersection_of_2_planes(plane1, plane2);
    //std::cout << int_line;

// 9) :
    //std::cout << "last point of alghoritm" << std::endl;

    std::vector<Vector> segment1 = interval_on_line(t1, dist1, int_line);
    //std::cout << "segment 1:\n"<< segment1[0] << segment1[1];

    std::vector<Vector> segment2 = interval_on_line(t2, dist2, int_line);
    //std::cout << "segment 2:\n"<< segment2[0] << segment2[1];

    //std::cout << "last ret\n";
    return intersect_of_intervals(segment1, segment2);

    // triangle_projection on int_line
    // intersect_of_intervals
}
}
