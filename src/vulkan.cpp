#include "app.hpp"
#include "triangle.hpp"
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <vector>

glm::vec3 vector_cast_vec3(Geo3D::Vector vec);
glm::vec3 get_normal(const Geo3D::Triangle& triangle);


int vulkan(const std::vector<Geo3D::Triangle>& triangles, std::vector<bool>& status) {

    std::vector<vulkan_engine::Model::Vertex> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(triangles.size() * 3);
    indices.reserve(triangles.size() * 3 * 2);

    glm::vec3 red  = {1.0f, 0.0f, 0.0f};
    glm::vec3 blue = {0.0f, 0.0f, 1.0f};
    glm::vec3 color;

    int verticle = 0;
    for (int i = 0; i != triangles.size(); ++i) {
        if (status[i]) {
            color = red;
        }
        else color = blue;

        vertices.push_back({vector_cast_vec3(triangles[i].v0_), color, get_normal(triangles[i])});
        vertices.push_back({vector_cast_vec3(triangles[i].v1_), color, get_normal(triangles[i])});
        vertices.push_back({vector_cast_vec3(triangles[i].v2_), color, get_normal(triangles[i])});
    }


    for (int i = 0; i != triangles.size() * 3; ++i) {
        indices.push_back(i);
    }

    for (int i = triangles.size() * 3 - 1; i != -1; --i) {
        indices.push_back(i);
    }

    vulkan_engine::Application app(vertices, indices);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

glm::vec3 vector_cast_vec3(Geo3D::Vector vec) {
    glm::vec3 tmp;
    tmp.x = vec.x_; tmp.y = vec.y_; tmp.z = vec.z_;

    return tmp;
}

glm::vec3 get_normal(const Geo3D::Triangle& triangle) {
    glm::vec3 side10, side20;

    side10.x = triangle.v1_.x_ - triangle.v0_.x_; 
    side10.y = triangle.v1_.y_ - triangle.v0_.y_;
    side10.z = triangle.v1_.z_ - triangle.v0_.z_;

    side20.x = triangle.v2_.x_ - triangle.v0_.x_;
    side20.y = triangle.v2_.y_ - triangle.v0_.y_;
    side20.z = triangle.v2_.z_ - triangle.v0_.z_;

    return glm::normalize(glm::cross(side10, side20));

}