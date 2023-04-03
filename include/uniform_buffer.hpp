#pragma once

#include "device.hpp"
#include "swap_chain.hpp"

#include <glm/glm.hpp>
#include "vulkan/vulkan.hpp"

#include <vector>
#include <vulkan/vulkan_structs.hpp>

namespace vulkan_engine {


class UniformBuffer {
public:

    UniformBuffer(Device & device, SwapChain & swapChain);
    ~UniformBuffer();

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer &operator=(const UniformBuffer&) = delete;

    std::vector<vk::Buffer> getUniformBuffers() const { return uniformBuffers_; }
    auto getSizeOfUniformBufferObject() const { return sizeof(UniformBufferObject); }

    std::vector<void*> getUniformBeffersMapped() const { return uniformBuffersMapped_; }

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec3 viewPos;
    };

private:
    
    Device & device_;
    SwapChain & swapChain_;

    std::vector<vk::Buffer> uniformBuffers_;
    std::vector<vk::DeviceMemory> uniformBuffersMemory_;
    std::vector<void*> uniformBuffersMapped_;
    
};
}