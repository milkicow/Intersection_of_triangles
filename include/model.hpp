#pragma once

#include "vulkan/vulkan.hpp"
#include "device.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace vulkan_engine {

class Model {
public:

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static vk::VertexInputBindingDescription getBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions();
};
   
    Model(Device & device);
    ~Model();

    std::vector<Vertex> vertices_;
    std::vector<uint16_t> indices_;

private:
    vk::Buffer vertexBuffer_;
    vk::DeviceMemory vertexBufferMemory_;
    vk::Buffer indexBuffer_;
    vk::DeviceMemory indexBufferMemory_;
    
    Device & device_;

    void createVertexBuffer();
    void createIndexBuffer();

};

}