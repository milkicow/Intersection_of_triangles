#include "intersect.hpp"
#include <chrono>

int main()
{   
    //auto start = std::chrono::steady_clock::now();

    Geo3D::receive_triangles();
    //Geo3D::receive_triangles_without_octree();

    //auto end = std::chrono::steady_clock::now();

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";

    return 0;
}