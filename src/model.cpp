#include "model.hpp"
#include "device.hpp"

namespace vulkan_engine {

Model::Model(Device &device) : device_(device) {
    createVertexBuffer();
    createIndexBuffer();
}

Model::~Model() {
    device_.getDevice().destroyBuffer(indexBuffer_, nullptr);
    device_.getDevice().freeMemory(indexBufferMemory_, nullptr);

    device_.getDevice().destroyBuffer(vertexBuffer_, nullptr);
    device_.getDevice().freeMemory(vertexBufferMemory_, nullptr);
}

void Model::createVertexBuffer() {
    vk::DeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    device_.createBuffer(bufferSize,vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    auto data = device_.getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices_.data(), (size_t) bufferSize);
    device_.getDevice().unmapMemory(stagingBufferMemory);

    device_.createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst  | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer_, vertexBufferMemory_);

    device_.copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);

    device_.getDevice().destroyBuffer(stagingBuffer);
    device_.getDevice().freeMemory(stagingBufferMemory);
}

void Model::createIndexBuffer() {
    vk::DeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    device_.createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    auto data = device_.getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices_.data(), (size_t) bufferSize);
    device_.getDevice().unmapMemory(stagingBufferMemory);

    device_.createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer_, indexBufferMemory_);

    device_.copyBuffer(stagingBuffer, indexBuffer_, bufferSize);

    device_.getDevice().destroyBuffer(stagingBuffer);
    device_.getDevice().freeMemory(stagingBufferMemory);
}

vk::VertexInputBindingDescription Model::Vertex::getBindingDescription() {
    vk::VertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

    return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 3> Model::Vertex::getAttributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[2].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
}

}