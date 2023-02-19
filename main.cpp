#include "intersect.hpp"

int main()
{   
    Geo3D::Vector v11(0, 0, 0);
    Geo3D::Vector v12(0, 0, 0);
    Geo3D::Vector v13(0, 0, 0);

    Geo3D::Triangle t1(v11, v12, v13);

    Geo3D::Vector v21(0, 0, 0);
    Geo3D::Vector v22(1, 1, 1);
    Geo3D::Vector v23(1, 1, 1);

    Geo3D::Triangle t2(v21, v22, v23);

    td::cout << Geo3D::triangles_intersection(t1, t2) << std::endl;

    return 0;
}