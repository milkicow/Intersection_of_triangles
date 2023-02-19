#include "intersect.hpp"

namespace Geo3D 
{


void compute_interval(const Triangle& triangle, const Vector& normal, double& min, double& max) {
    min = max = normal * triangle[0];

    for (int i = 1; i < 3; ++i) {
        double value = normal * triangle[i];
        if      (value < min) min = value;
        else if (value > max) max = value;
    }
}

bool triangls_intersection_2D(const Triangle& t0, const Triangle& t1, Ort u, Ort v) {

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

Vector point_of_intersection(Line line1, Line line2) {
    Vector vector;

    double tmp = det(line1.direction_, line2.direction_, cross(line1.direction_, line2.direction_));
    double determinant = det(line2.point_ - line1.point_, line2.direction_, cross(line1.direction_, line2.direction_));

    double t = determinant / tmp;

    vector.x_ = line1.point_[x] + t * line1.direction_[x];
    vector.y_ = line1.point_[y] + t * line1.direction_[y];
    vector.z_ = line1.point_[z] + t * line1.direction_[z];

    return vector;
}

bool intersection(const Vector& point1, const Vector& point2) // point and point
{
    return point1 == point2;
}

bool intersection(const Vector& point, const Segment& segment)
{   
    return (segment.point_belongs(point));
}

bool intersection(const Vector& point, const Triangle& triangle) {

    Plane plane(triangle);
    if (point_plane_dist(point, plane) != 0) return false;

    Vector start(0, 0, 0);
    Segment segment(start, point);
    return intersection(segment, triangle);
}

bool intersection(const Segment& segment1, const Segment& segment2) {
    Line line1 (segment1.v1_ - segment1.v0_, segment1.v0_);
    Line line2 (segment2.v1_ - segment2.v0_, segment2.v0_);

    if (!intersection(line1, line2)) return false;
    else {
        Vector general_point = point_of_intersection(line1, line2);
        return (segment1.point_belongs(general_point) && segment2.point_belongs(general_point));
    }
}
bool intersection(const Segment& segment, const Triangle& triangle) {

    Vector p, s, q, side1, side2;
    double t, u, v, tmp;
    Line line(segment.v1_ - segment.v0_, segment.v0_); 

    side1 = triangle.v1_ - triangle.v0_;
    side2 = triangle.v2_ - triangle.v0_;

    p = cross(line.direction_, side2);
    tmp = p * side1;
    if (is_equal(tmp, 0)) {

        Segment segment1(triangle.v0_, triangle.v1_);
        Segment segment2(triangle.v0_, triangle.v2_);
        Segment segment3(triangle.v1_, triangle.v2_);

        return intersection(segment, segment1) || intersection(segment, segment2) || intersection(segment, segment3);
    }   

    tmp = 1 / tmp;
    s = line.point_ - triangle.v0_;

    u = tmp * s * p;
    if (u < 0 || u > 1) return false;

    q = cross(s, side1);
    v = tmp * line.direction_ * q;
    if (v < 0 || v > 1) return false;

    t = tmp * side2 * q;

    Vector intersection = line.point_ + t * line.direction_;
    return segment.point_belongs(intersection);
}

bool triangles_intersection(const Triangle& triangle1, const Triangle& triangle2) {

    std::cout << "triangle1 = " << triangle1.status_ << std::endl;
    std::cout << "triangle2 = " << triangle2.status_ << std::endl;
    if (triangle1.status_ != Triangle::triangle || triangle2.status_ != Triangle::triangle) {
        return degenerate_intersection(triangle1, triangle2);
    }
    LOX
    
// 2) :
    Plane plane1(triangle1);
    //std::cout << plane1;
    LOX

// 3) :
    Vector dist2 = triangle_plane_dist(triangle2, plane1);
    //std::cout << "dist2 = " << dist2;
    LOX

// 4) :
    int dist2_sign = cmp_zero(dist2.x_) + cmp_zero(dist2.y_) + cmp_zero(dist2.z_);
    if (dist2_sign == 3 || dist2_sign == -3) return false;
    //std::cout << dist2_sign << std::endl;
    LOX
// 5) :
    Plane plane2(triangle2);
    //std::cout << "plane2 = "<< plane2;
    LOX
// 6) :
    //std::cout << plane1.normal() << plane2.normal();
    
    plane1.normalized();
    plane2.normalized();

    //std::cout << plane1.normal() << plane2.normal();

    if (is_equal(plane1.normal(), plane2.normal()) || is_equal(plane1.normal(), -plane2.normal())) {
        if (plane1.d_ != plane2.d_) return false;
        else {
            //std::cout << "plane1.normal() == plane2.normal()" << std::endl;

            Ort ort = max_ort_component(plane1.normal());
            
            Ort u, v;
            switch(ort)
            {
                case Ort::x: u = Ort::y; v = Ort::z; break;
                case Ort::y: u = Ort::x; v = Ort::z; break;
                case Ort::z: u = Ort::x; v = Ort::y; break;
                default: std::cout << "Can't define new orts\n"; exit(EXIT_FAILURE);
            }

            return triangls_intersection_2D(triangle1, triangle2, u, v);
        }
    }
    LOX

// 7) :
    //std::cout << "plane2.2 = " << plane2;
    Vector dist1 = triangle_plane_dist(triangle1, plane2);

    //std::cout << "dist1 = " << dist1;

    int dist1_sign = cmp_zero(dist1.x_) + cmp_zero(dist1.y_) + cmp_zero(dist1.z_);
    if (dist1_sign == 3 || dist1_sign == -3) return false;

    LOX
    //std::cout << dist1_sign << std::endl;

// 8) :
    Line int_line = intersection_of_2_planes(plane1, plane2);
    LOX
    //std::cout << "int_line = " << int_line;

// 9) :
    //std::cout << "last point of alghoritm" << std::endl;

    Segment segment1 = interval_on_line(triangle1, dist1, int_line);
    //std::cout << "segment 1:\n"<< segment1.v0_ << segment1.v1_;
    LOX

    Segment segment2 = interval_on_line(triangle2, dist2, int_line);
    //std::cout << "segment 2:\n"<< segment2.v0_ << segment2.v1_;
    LOX
    //std::cout << "last ret\n";
    return intersect_of_intervals(segment1, segment2);

    // triangle_projection on int_line
    // intersect_of_intervals
}

bool degenerate_intersection(const Triangle& triangle1, const Triangle& triangle2) {

    switch(triangle1.status_) 
    {
        case Triangle::triangle:
        {
            if (triangle2.status_ == Triangle::segment) {
                std::cout << "triangle and segment\n";
                Segment segment2(triangle2);
                return intersection(segment2, triangle1);
            }
            else if (triangle2.status_ == Triangle::point) {
                std::cout << "triangle and point\n";
                Vector point2 = triangle2.v0_;
                return intersection(point2, triangle1);
            }
            break;
        }
        case Triangle::segment: 
        {
            Segment segment1(triangle1);
            if (triangle2.status_ == Triangle::triangle) {
                std::cout << "segment and triangle\n";
                return intersection(segment1, triangle2);
            }
            else if (triangle2.status_ == Triangle::segment) {
                std::cout << "segment and segment\n";
                Segment segment2(triangle2);
                return intersection(segment1, segment2);
            }
            else if (triangle2.status_ == Triangle::point) {
                std::cout << "segment and point\n";
                Vector point2 = triangle2.v0_;
                return intersection(point2, segment1);
            }
            break;
        }
        case Triangle::point:
        {
            Vector point1 = triangle1.v0_; 
            if (triangle2.status_ == Triangle::triangle) {
                std::cout << "point and triangle\n";
                return intersection(point1, triangle2);
            }
            else if (triangle2.status_ == Triangle::segment) {
                std::cout << "point and segment\n";
                Segment segment2(triangle2);
                return intersection(point1, segment2);
            }
            else if (triangle2.status_ == Triangle::point) {
                std::cout << "point and point\n";
                Vector point2 = triangle2.v0_;
                return intersection(point1, point2);
            }
            break;
        }
        default:
        {
            std::cout << "Can't define status of triangle1\n";
            exit(EXIT_FAILURE);
        }
    }
    return false;
}





// bool intersection_of_points(Triangle t1, Triangle t2) {
//     return t1 == t2;
// }
// bool intersection_of_point_and_line(Triangle t1, Triangle t2) {
//     return (t2.)
// }
// bool intersection_of_point_and_triangle(Triangle t1, Triangle t2) {}
// bool intersection_of_lines(Triangle t1, Triangle t2) {}
// bool intersection_of_line_and_triangle(Triangle t1, Triangle t2) {}
// bool intersection_of_2_triangles(Triangle t1, Triangle t2) {

// 1) DO ! : check t1 and t2 on degeneracy !
//     switch (t1.status_)
//     {
//         case Triangle::point:
//         {   
//             if      (t2.status_ == Triangle::point) return intersection_of_points(t1, t2);
//             else if (t2.status_ == Triangle::line)  return intersection_of_point_and_line(t1, t2);
//             else if (t2.status_ == Triangle::triangle) return intersection_of_point_and_triangle(t1, t2);
//             break;
//         }
//         case Triangle::line:
//         {
//             if      (t2.status_ == Triangle::point) return intersection_of_point_and_line(t2, t1);
//             else if (t2.status_ == Triangle::line)  return intersection_of_lines(t1, t2);
//             else if (t2.status_ == Triangle::triangle) return intersection_of_line_and_triangle(t1, t2);
//             break;
//         }
//         case Triangle::triangle:
//         {
//             if      (t2.status_ == Triangle::point) return intersection_of_point_and_triangle(t2, t1);
//             else if (t2.status_ == Triangle::line)  return intersection_of_line_and_triangle(t2, t1);
//             else if (t2.status_ == Triangle::triangle) return intersection_of_triangle_and_triangle(t1, t2);
//             break;
//         }

//     }
// }

}
