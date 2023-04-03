#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "uniform_buffer.hpp"

#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_handles.hpp>



namespace vulkan_engine {

class DescriptorSetLayout {
public:

    DescriptorSetLayout(Device & device);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout&) = delete;

    vk::DescriptorSetLayout* getDescriptorSetLayout_() { return &descriptorSetLayout_; }
    
private:
    vk::DescriptorSetLayout descriptorSetLayout_;
    
    Device & device_;

};


class DescriptorPool {
public:
    
    DescriptorPool(Device & device, SwapChain & swapChain);
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool &operator=(const DescriptorPool&) = delete;

    vk::DescriptorPool getDescriptorPool() const { return descriptorPool_; }


private:
    vk::DescriptorPool descriptorPool_;

    Device & device_;
    SwapChain & swapChain_;
};


class DescriptorSets{
public:
    
    DescriptorSets(Device & device, SwapChain & swapChain, UniformBuffer & uniformBuffer_, DescriptorSetLayout & descripterSetLayout, DescriptorPool & descripterPool);

    DescriptorSets(const DescriptorSets&) = delete;
    DescriptorSets &operator=(const DescriptorSets&) = delete;

    std::vector<vk::DescriptorSet> getDescriptorSets() const { return descriptorSets_; }

private:
    std::vector<vk::DescriptorSet> descriptorSets_;
    
    Device & device_;
    SwapChain & swapChain_;
    UniformBuffer & uniformBuffer_;

    DescriptorSetLayout & descriptorSetLayout_;
    DescriptorPool & descriptorPool_;
};

}