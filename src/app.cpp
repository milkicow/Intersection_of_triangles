#include "app.hpp"

namespace vulkan_engine {

void Application::mainLoop() {
    glfwGetCursorPos(window.getGLFWwindow(), &prev_x, &prev_y);
    glfwSetKeyCallback (window.getGLFWwindow(), key_callback);
    glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_button_callback);

    while (!window.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }

    device.getDevice().waitIdle();
}

void Application::drawFrame() {

    vk::resultCheck(device.getDevice().waitForFences(1, &swapChain.getInFlightFences()[commandBuffers.currentFrame_], VK_TRUE, UINT64_MAX), "failed to wait for Fences!");

    uint32_t imageIndex;
    vk::Result result = device.getDevice().acquireNextImageKHR(swapChain.getSwapChain(), UINT64_MAX, swapChain.getImageAvailableSemaphores()[commandBuffers.currentFrame_], nullptr, &imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        swapChain.recreate();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    uniformBuffer.update(commandBuffers.currentFrame_);

    vk::resultCheck(device.getDevice().resetFences(1, &swapChain.getInFlightFences()[commandBuffers.currentFrame_]), "failed to reset Fences!");

    commandBuffers.getCommandBuffers()[commandBuffers.currentFrame_].reset();
    commandBuffers.record(commandBuffers.getCommandBuffers()[commandBuffers.currentFrame_], imageIndex);

    vk::Semaphore waitSemaphores[] = {swapChain.getImageAvailableSemaphores()[commandBuffers.currentFrame_]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::Semaphore signalSemaphores[] = {swapChain.getRenderFinishedSemaphores()[commandBuffers.currentFrame_]};

    vk::SubmitInfo submitInfo{
        1, waitSemaphores, waitStages,
        1, &commandBuffers.getCommandBuffers()[commandBuffers.currentFrame_],
        1, signalSemaphores
    };

    vk::resultCheck(device.getGraphicsQueue().submit(1, &submitInfo, swapChain.getInFlightFences()[commandBuffers.currentFrame_]), "failed to submit draw command buffer!");

    vk::SwapchainKHR swapChains[] = { swapChain.getSwapChain() };

    vk::PresentInfoKHR presentInfo{
        signalSemaphores,
        swapChains,
        imageIndex
    };

    result = device.getPresentQueue().presentKHR(&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || window.wasFrameBufferResized()) {
        window.resetFrameBufferRisizedFlag();
        swapChain.recreate();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    commandBuffers.currentFrame_ = (commandBuffers.currentFrame_ + 1) % swapChain.MAX_FRAMES_IN_FLIGHT;
}


//void Application::cleanup() {
////        swapChain.cleanup();
//
////        for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) {
////            device.getDevice().destroyBuffer(uniformBuffers[i], nullptr);
////            device.getDevice().freeMemory(uniformBuffersMemory[i], nullptr);
////        }
////        device.getDevice().destroyDescriptorPool(descriptorPool, nullptr);
////        device.getDevice().destroyDescriptorSetLayout(descriptorSetLayout, nullptr);
//
////        device.getDevice().destroyBuffer(indexBuffer, nullptr);
////        device.getDevice().freeMemory(indexBufferMemory, nullptr);
////
////        device.getDevice().destroyBuffer(vertexBuffer, nullptr);
////        device.getDevice().freeMemory(vertexBufferMemory, nullptr);
//
////        device.getDevice().destroyPipeline(graphicsPipeline, nullptr);
////        device.getDevice().destroyPipelineLayout(pipelineLayout, nullptr);
//}

void Application::mouse_button_callback (GLFWwindow* window, int button, int action, int mods) noexcept {
    if (action == GLFW_PRESS)        Input::press_mouse_button(button);
    else if (action == GLFW_RELEASE) Input::release_mouse_button(button);

}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) Input::press_keyboard_key(key);
    else if (action == GLFW_RELEASE)                   Input::release_keyboard_key(key);
}

}