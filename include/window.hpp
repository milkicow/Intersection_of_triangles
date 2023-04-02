#pragma once

#include <_types/_uint32_t.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <sys/_types/_u_int32_t.h>
#include <string>

namespace vulkan_engine {

class Window {
public:
    Window(uint32_t width, uint32_t height, std::string windowName) : width_(width), height_(height), windowName_(windowName) { initWindow(); };
    ~Window() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    };

    Window(const Window&) = delete;
    Window & operator=(const Window &) = delete;

    void createWindowSurface(VkInstance instance, VkSurfaceKHR * surface);

    uint32_t getWidth()  const { return width_; }
    uint32_t getHeight() const { return height_; }

    void resetFrameBufferRisizedFlag() { frameBufferResized = false; }
    bool wasWindowResized() { return frameBufferResized; }

    GLFWwindow * getGLFWwindow() { return window_; }
    bool shouldClose() { return glfwWindowShouldClose(window_); }

private:
    uint32_t width_, height_;
    std::string windowName_;
    GLFWwindow * window_;

    bool frameBufferResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void initWindow();
};

}