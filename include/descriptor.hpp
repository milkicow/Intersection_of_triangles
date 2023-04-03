#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_handles.hpp>



namespace vulkan_engine {

class DescriptorSetLayout {
public:

    DescriptorSetLayout(Device & device);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout&) = delete;

    vk::DescriptorSetLayout getDescriptorSetLayout_() const { return descriptorSetLayout_; }
    
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
    
    DescriptorSets(Device & device, SwapChain & swapChain, DescriptorSetLayout & descripterSetLayout, DescriptorPool & descripterPool);
    ~DescriptorSets();

    DescriptorSets(const DescriptorSets&) = delete;
    DescriptorSets &operator=(const DescriptorSets&) = delete;

private:
    std::vector<vk::DescriptorSet> descriptorSets_;
    
    Device & device_;
    SwapChain & swapChain_;
    DescriptorSetLayout & descriptorSetLayout_;
    DescriptorPool & descriptorPool_;
};

}