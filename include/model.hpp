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
   
    Model(Device & device, std::vector<Model::Vertex> vertices, std::vector<uint16_t> indices);
    ~Model();

    Model(const Model&) = delete;
    Model &operator=(const Model&) = delete;

    std::vector<Vertex> vertices_;
    std::vector<uint16_t> indices_;

    const vk::Buffer& getVertexBuffer() const & { return vertexBuffer_; }
    const vk::Buffer& getIndexBuffer()  const & { return indexBuffer_;  }
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