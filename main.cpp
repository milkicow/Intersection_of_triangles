#include "intersect.hpp"

int main()
{   
    Geo3D::Vector v11(1, 1, 1);
    Geo3D::Vector v12(1, 1, 1);
    Geo3D::Vector v13(1, 1, 1);

    Geo3D::Triangle t1(v11, v12, v13);

    Geo3D::Vector v21(0, 0, 0);
    Geo3D::Vector v22(0, 0, 0);
    Geo3D::Vector v23(0, 0, 0);

    Geo3D::Triangle t2(v21, v22, v23);

    std::cout << Geo3D::intersection(t1, t2) << std::endl;

    return 0;
}