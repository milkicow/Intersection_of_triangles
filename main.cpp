#include "intersect.hpp"
#include <chrono>

int main()
{   
    try {
        //auto start = std::chrono::steady_clock::now();
        
        Geo3D::receive_triangles();
        //Geo3D::receive_triangles_without_octree();

        //auto end = std::chrono::steady_clock::now();

        //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }
    catch (std::invalid_argument& error) {
        std::cout << error.what() << std::endl;
    }
    return 0;
}