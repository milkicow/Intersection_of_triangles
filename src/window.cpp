#include "window.hpp"
#include <stdexcept>

namespace vulkan_engine {

void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(width_, height_, windowName_.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto vulkan_engine_window = reinterpret_cast<Window *> (glfwGetWindowUserPointer(window));
    vulkan_engine_window->frameBufferResized = true;
    vulkan_engine_window->width_  = width;
    vulkan_engine_window->height_ = height;
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR * surface) {
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
    }
}

}
