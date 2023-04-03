#include "descriptor.hpp"
#include "device.hpp"

namespace vulkan_engine {

DescriptorSetLayout::DescriptorSetLayout(Device & device) : device_(device) {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{
        0,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eAllGraphics
    };

    vk::DescriptorSetLayoutCreateInfo layoutInfo{
        vk::DescriptorSetLayoutCreateFlags(),
        1,
        &uboLayoutBinding
    };

    vk::resultCheck(device_.getDevice().createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout_), "failed to create descriptor set layout!");

}

DescriptorSetLayout::~DescriptorSetLayout() {
  device_.getDevice().destroyDescriptorSetLayout(descriptorSetLayout_, nullptr);
}

DescriptorPool::DescriptorPool(Device & device, SwapChain & swapChain) : device_(device), swapChain_(swapChain) {
    vk::DescriptorPoolSize poolSize{
        vk::DescriptorType::eUniformBuffer,
        static_cast<uint32_t>(swapChain_.MAX_FRAMES_IN_FLIGHT)
    };

    vk::DescriptorPoolCreateInfo poolInfo{
        vk::DescriptorPoolCreateFlags(),
        static_cast<uint32_t>(swapChain_.MAX_FRAMES_IN_FLIGHT),
        1,
        &poolSize
    };

    vk::resultCheck(device.getDevice().createDescriptorPool(&poolInfo, nullptr, &descriptorPool_), "failed to create descriptor pool!");
    
}

DescriptorPool::~DescriptorPool() {
        device_.getDevice().destroyDescriptorPool(descriptorPool_, nullptr);
}


DescriptorSets::DescriptorSets(Device & device, SwapChain & swapChain, DescriptorSetLayout & descripterSetLayout, DescriptorPool & descripterPool) :
    device_(device), swapChain_(swapChain), descriptorSetLayout_(descripterSetLayout), descriptorPool_(descripterPool) {
    
    std::vector<vk::DescriptorSetLayout> layouts(swapChain.MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_.getDescriptorSetLayout_());
    vk::DescriptorSetAllocateInfo allocInfo{
        descriptorPool_.getDescriptorPool(),
        static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT),
        layouts.data()
    };

    descriptorSets_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
    vk::resultCheck(device.getDevice().allocateDescriptorSets(&allocInfo, descriptorSets_.data()), "failed to allocate descriptor sets!");

    for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo bufferInfo{
            uniformBuffers[i],
            0,
            sizeof(UniformBufferObject)
        };

        vk::WriteDescriptorSet descriptorWrite{
            descriptorSets_[i],
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &bufferInfo
        };

        device.getDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}

}
