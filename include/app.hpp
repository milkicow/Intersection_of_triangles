#pragma once

#include "model.hpp"
#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"
#include "uniform_buffer.hpp"
#include "command_buffer.hpp"
#include "camera.hpp"
#include "input.hpp"
#include <_types/_uint16_t.h>
#include <vector>


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vulkan_engine {

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

static double prev_x = 0.0;
static double prev_y = 0.0;
static bool lpress = false;

class Application {
public:
    void run() {
        //initWindow();
        //initVulkan();
        mainLoop();
        //cleanup();
    }

    Application(std::vector<Model::Vertex> vertices, std::vector<uint16_t> indices) : model(device, vertices, indices) {}

private:
    Camera camera{};
    Window window { WIDTH, HEIGHT, "vulkan" };
    Device device { window };
    SwapChain swapChain { window, device };
    DescriptorSetLayout descriptorSetLayout { device };
    Pipeline pipeline { device, swapChain, descriptorSetLayout, "../../shaders/vert.spv", "../../shaders/frag.spv" };
    Model model;
    UniformBuffer uniformBuffer { device, swapChain, camera, window };
    DescriptorPool descriptorPool { device, swapChain };
    DescriptorSets descriptorSets { device, swapChain, uniformBuffer, descriptorSetLayout, descriptorPool };
    CommandBuffers commandBuffers { device, swapChain, pipeline, model, descriptorSets };
    
    void mainLoop();
    void drawFrame();
//    void cleanup();

    static void mouse_button_callback (GLFWwindow* window, int button, int action, int mods) noexcept;
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
}