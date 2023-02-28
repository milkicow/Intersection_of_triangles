#include "gtest/gtest.h"
#include <fstream>
#include "intersect.hpp"

void TestBody(const std::string& path, int testNumber) {

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Problem with open file " << testNumber <<  "\n";
        std::cerr << path << '\n';
        std::cerr << "test FAILURE\n";
        file.close();
        return;
    }

    for (int i = 0; i < testNumber; ++i) {
        Geo3D::Triangle first;
        Geo3D::Triangle second;
        bool actual_result;

        file >> first >> second >> actual_result;
        
        auto func_res = Geo3D::triangles_intersection(first, second);

        EXPECT_EQ(func_res, actual_result);
        
        if (func_res == actual_result) {
            std::cout << "\ntest " << i << " PASSED \n\n";
        }
        else {
            std::cout << "\ntest " << i << " FAILED \n\n";
        }
    }
    file.close();  
}

TEST(TestTriangles, segment_and_point) {       
    const int NUMBER_OF_TEST = 4;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/segment_and_point.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}

TEST(TestTriangles, point_and_point) {       
    const int NUMBER_OF_TEST = 3;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/point_and_point.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}

TEST(TestTriangles, triangle_and_point) {       
    const int NUMBER_OF_TEST = 3;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/triangle_and_point.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}

TEST(TestTriangles, segment_and_segment) {       
    const int NUMBER_OF_TEST = 11;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/segment_and_segment.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}

TEST(TestTriangles, triangle_and_segment) {       
    const int NUMBER_OF_TEST = 9;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/triangle_and_segment.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}

TEST(TestTriangles, triangle_and_triangle) {       
    const int NUMBER_OF_TEST = 18;
    std::string path = static_cast<std::string> (PROJECT_DIR_PATH) + "/test/triangle_and_triangle.txt";
    ::TestBody(path, NUMBER_OF_TEST);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}