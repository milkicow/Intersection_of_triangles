#include "command_buffer.hpp"
#include "descriptor.hpp"
#include "model.hpp"
#include "pipeline.hpp"

namespace vulkan_engine {

CommandBuffers::CommandBuffers(Device & device, SwapChain & swapChain, Pipeline & pipeline, Model & model, DescriptorSets & descriptorSets)
    : device_(device), swapChain_(swapChain), pipeline_(pipeline), model_(model), descriptorSets_(descriptorSets) {

    commandBuffers_.resize(swapChain_.MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocInfo{
        device_.getCommandPool(),
        vk::CommandBufferLevel::ePrimary,
        (uint32_t) commandBuffers_.size()
    };

    vk::resultCheck(device_.getDevice().allocateCommandBuffers(&allocInfo, commandBuffers_.data()), "failed to allocate command buffers!");
}

void CommandBuffers::record(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo{};

    vk::resultCheck(commandBuffer.begin(&beginInfo), "failed to begin recording command buffer!");

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].setColor({0.0f, 0.0f, 0.0f, 1.0f});
    clearValues[1].setDepthStencil({1.0f, 0});

    vk::RenderPassBeginInfo renderPassInfo{
        swapChain_.getRenderPass(),
        swapChain_.getFramebuffers()[imageIndex],
        {{0,0}, swapChain_.getExtent()},
        clearValues
    };

    commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_.getGraphicsPipeline());

    vk::Viewport viewport{
        0.0f, 0.0f,
        static_cast<float>(swapChain_.getExtent().width),
        static_cast<float>(swapChain_.getExtent().height),
        0.0f, 1.0f
    };
    commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor{ {0, 0}, swapChain_.getExtent() };
    commandBuffer.setScissor(0, 1, &scissor);

    vk::Buffer vertexBuffers[] = {model_.getVertexBuffer()};
    vk::DeviceSize offsets[] = {0};
    commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffer.bindIndexBuffer(model_.getIndexBuffer(), 0, vk::IndexType::eUint16);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_.getPipelineLayout(), 0, 1, &descriptorSets_.getDescriptorSets()[currentFrame_], 0, nullptr);
    commandBuffer.drawIndexed(static_cast<uint32_t>(model_.indices_.size()), 1, 0, 0, 0);

    commandBuffer.endRenderPass();
    commandBuffer.end();
}

}