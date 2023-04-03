#pragma once

#include <vector>

#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "descriptor.hpp"
#include "vulkan/vulkan.hpp"



namespace vulkan_engine {

class CommandBuffers {
public:
    
    uint32_t currentFrame_ = 0;
    
    CommandBuffers(Device & device, SwapChain & swapChain, Pipeline & pipeline, Model & model, DescriptorSets & descriptorSets);

    CommandBuffers(const CommandBuffers&) = delete;
    CommandBuffers &operator=(const CommandBuffers&) = delete;

    std::vector<vk::CommandBuffer> getCommandBuffers() { return commandBuffers_; }

    void record(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
  
private:
    std::vector<vk::CommandBuffer> commandBuffers_;
    
    Device & device_;
    SwapChain & swapChain_;
    Pipeline & pipeline_;
    Model & model_;
    DescriptorSets & descriptorSets_;
    

};

}