#include "uniform_buffer.hpp"

namespace vulkan_engine {

UniformBuffer::UniformBuffer(Device & device, SwapChain & swapChain) : device_(device), swapChain_(swapChain) {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
        device.createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers_[i], uniformBuffersMemory_[i]);

        uniformBuffersMapped_[i] = device.getDevice().mapMemory(uniformBuffersMemory_[i], 0, bufferSize);
    }
}

UniformBuffer::~UniformBuffer() {
  for (size_t i = 0; i < swapChain_.MAX_FRAMES_IN_FLIGHT; i++) {
        device_.getDevice().destroyBuffer(uniformBuffers_[i], nullptr);
        device_.getDevice().freeMemory(uniformBuffersMemory_[i], nullptr);
    }
}


}