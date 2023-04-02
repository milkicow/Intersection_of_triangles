#include "window.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_enums.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <array>
#include <algorithm>
#include <chrono>

#include "triangle.hpp"
#include "vector.hpp"
#include "camera.hpp"
#include "input.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


void CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger) {
    vk::DispatchLoaderDynamic dynamic_loader{ instance, vkGetInstanceProcAddr };
    vk::resultCheck(instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, pDebugMessenger, dynamic_loader), "failed to create debug utils messenger");
}

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator) {
    vk::DispatchLoaderDynamic dynamic_loader{ instance, vkGetInstanceProcAddr };
    instance.destroyDebugUtilsMessengerEXT(debugMessenger, pAllocator, dynamic_loader);
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

// glm::vec3 viewer_position = glm::vec3(2.0f, 2.0f, 2.0f);
// glm::vec3 view_position =  glm::vec3(0.0f, 0.0f, 0.0f);
// static glm::vec3 camera_direction = glm::normalize (glm::vec3 {-2.0f, -2.0f, -2.0f});
// static glm::vec3 camera_up (0.0f, 0.0f, 1.0f);

static double prev_x = 0.0;
static double prev_y = 0.0;
static bool lpress = false;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
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
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 viewPos;
};

std::vector<Vertex> vertices; // = {
//     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
//     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
// };
std::vector<uint16_t> indices; // = 
// {
//     0, 1, 2, 2, 3, 0
// };

using namespace vulkan_engine;

class HelloTriangleApplication {
public:
    void run() {
        //initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
//    GLFWwindow* window;
    Window window{ WIDTH, HEIGHT, "vulkan" };

    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;
    vk::SurfaceKHR surface;

    vk::PhysicalDevice physicalDevice = nullptr;
    vk::Device device;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::SwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat;
    vk::Extent2D swapChainExtent;
    std::vector<vk::ImageView> swapChainImageViews;
    std::vector<vk::Framebuffer> swapChainFramebuffers;

    vk::RenderPass renderPass;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    vk::CommandPool commandPool;

    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;

    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    vk::DescriptorPool descriptorPool;
    std::vector<vk::DescriptorSet> descriptorSets;

    std::vector<vk::CommandBuffer> commandBuffers;

    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;

    bool framebufferResized = false;
    uint32_t currentFrame = 0;

    Camera camera{};

//    void initWindow() {
//        glfwInit();
//
//        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//
//        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
//        glfwSetWindowUserPointer(window, this);
//        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
//    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        window.createWindowSurface(instance, reinterpret_cast<VkSurfaceKHR *>(&surface));
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createCommandPool();
        createDepthResources();
        createFramebuffers();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        createSyncObjects();
    }

    void mainLoop() {
        glfwGetCursorPos(window.getGLFWwindow(), &prev_x, &prev_y);
        glfwSetKeyCallback (window.getGLFWwindow(), key_callback);
        glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_button_callback);
        //glfwSetCursorPosCallback (window, cursor_position_callback);

        while (!window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        device.waitIdle();
    }

    void cleanup() {
        cleanupSwapChain();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.destroyBuffer(uniformBuffers[i], nullptr);
            device.freeMemory(uniformBuffersMemory[i], nullptr);
        }
        device.destroyDescriptorPool(descriptorPool, nullptr);
        device.destroyDescriptorSetLayout(descriptorSetLayout, nullptr);

        device.destroyBuffer(indexBuffer, nullptr);
        device.freeMemory(indexBufferMemory, nullptr);

        device.destroyBuffer(vertexBuffer, nullptr);
        device.freeMemory(vertexBufferMemory, nullptr);

        device.destroyPipeline(graphicsPipeline, nullptr);
        device.destroyPipelineLayout(pipelineLayout, nullptr);
        device.destroyRenderPass(renderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.destroySemaphore(renderFinishedSemaphores[i], nullptr);
            device.destroySemaphore(imageAvailableSemaphores[i], nullptr);
            device.destroyFence(inFlightFences[i], nullptr);
        }

        device.destroyCommandPool(commandPool, nullptr);

        device.destroy();

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        instance.destroySurfaceKHR(surface, nullptr);
        instance.destroy();

        window.~Window();
    }

    void cleanupSwapChain() {
        device.destroyImageView(depthImageView, nullptr);

        device.destroyImage(depthImage, nullptr);
        device.freeMemory(depthImageMemory, nullptr);

        for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
            device.destroyFramebuffer(swapChainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            device.destroyImageView(swapChainImageViews[i], nullptr);
        }
        device.destroySwapchainKHR(swapChain, nullptr);
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);
            glfwWaitEvents();
        }
        device.waitIdle();

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createDepthResources();
        createFramebuffers();
    }


    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        vk::ApplicationInfo appInfo("Hello Triangle", VK_API_VERSION_1_0, "Engine", VK_API_VERSION_1_0, VK_API_VERSION_1_0);

        auto extensions = getRequiredExtensions();
        vk::InstanceCreateInfo createInfo(vk::InstanceCreateFlags(), &appInfo, validationLayers, extensions);
        createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (vk::DebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
        }

        vk::resultCheck(vk::createInstance(&createInfo, nullptr, &instance), "failed to create instance!");

}

    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
      createInfo = vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(),
                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                 vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                 debugCallback);
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
    }

//    void createSurface() {
////      // if SEGFAULT change reinterpret_cast on VkSurfaceKHR tmp; surface = tmp; !
////        if (glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS) {
////
////            throw std::runtime_error("failed to create window surface!");
////        }
//
//
//    }

    void pickPhysicalDevice() {
        auto devices = instance.enumeratePhysicalDevices();

        auto iter = std::find_if(devices.begin(), devices.end(), [&](auto& device) {
            return isDeviceSuitable(device);
        });

        if (iter != devices.end()) physicalDevice = *iter;
        else throw std::runtime_error("failed to find a suitable GPU!");
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};
        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, validationLayers, deviceExtensions, &deviceFeatures);

        vk::resultCheck(physicalDevice.createDevice(&createInfo, nullptr, &device), "failed to create logical device!");
        device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
        device.getQueue(indices.presentFamily.value(), 0, &presentQueue);
    }

    bool isDeviceSuitable(vk::PhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport(vk::PhysicalDevice device) {
        auto availableExtensions = device.enumerateDeviceExtensionProperties();
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) {
        QueueFamilyIndices indices;
        auto queueFamilies = device.getQueueFamilyProperties();

        int i = 0;

        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;

            vk::Bool32 presentSupport = false;
            vk::resultCheck(device.getSurfaceSupportKHR(i, surface, &presentSupport), "failed to get surface support KHR");

            if (presentSupport) indices.presentFamily = i;
            if (indices.isComplete()) break;
            ++i;
        }
        return indices;
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(), surface, imageCount, surfaceFormat.format,
            surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment
            );
        
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = nullptr;

        vk::resultCheck(device.createSwapchainKHR(&createInfo, nullptr, &swapChain), "failed to create swap chain!");

        swapChainImages = device.getSwapchainImagesKHR(swapChain);
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (uint32_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, vk::ImageAspectFlagBits::eColor);
        }
    }

    void createRenderPass() {
        vk::AttachmentDescription colorAttachment(
            vk::AttachmentDescriptionFlags(),
            swapChainImageFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
      );

      vk::AttachmentDescription depthAttachment(
          vk::AttachmentDescriptionFlags(),
          findDepthFormat(),
          vk::SampleCountFlagBits::e1,
          vk::AttachmentLoadOp::eClear,
          vk::AttachmentStoreOp::eDontCare,
          vk::AttachmentLoadOp::eDontCare,
          vk::AttachmentStoreOp::eDontCare,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::eAttachmentOptimal
      );

        vk::AttachmentReference colorAttachmentRef{ 0, vk::ImageLayout::eColorAttachmentOptimal };

        vk::AttachmentReference depthAttachmentRef{ 1, vk::ImageLayout::eStencilAttachmentOptimal};

        vk::SubpassDescription subpass {
            vk::SubpassDescriptionFlags(),
            vk::PipelineBindPoint::eGraphics,
            0,
            nullptr,
            1,
            &colorAttachmentRef,
            nullptr,
            &depthAttachmentRef
        };

        vk::SubpassDependency dependency {
          VK_SUBPASS_EXTERNAL,
          0,
          vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
          vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
          vk::AccessFlags(),
          vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
        };


        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        vk::RenderPassCreateInfo renderPassInfo{
            vk::RenderPassCreateFlags(),
            attachments,
            subpass,
            dependency
        };


        vk::resultCheck(device.createRenderPass(&renderPassInfo, nullptr, &renderPass), "failed to create render pass!");
    }

    void createDescriptorSetLayout() {
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

        vk::resultCheck(device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout), "failed to create descriptor set layout!");

    }

    void createGraphicsPipeline() {
        auto vertShaderCode = readFile("../../shaders/vert.spv");
        auto fragShaderCode = readFile("../../shaders/frag.spv");

        vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eVertex,
            vertShaderModule,
            "main"
        };

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
            vk::PipelineShaderStageCreateFlags(),
            vk::ShaderStageFlagBits::eFragment,
            fragShaderModule,
            "main"
        };

        vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
            vk::PipelineVertexInputStateCreateFlags(),
            1,
            &bindingDescription,
            static_cast<uint32_t>(attributeDescriptions.size()),
            attributeDescriptions.data()
        };

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
            vk::PipelineInputAssemblyStateCreateFlags(),
            vk::PrimitiveTopology::eTriangleList,
            VK_FALSE
        };

        vk::PipelineViewportStateCreateInfo viewportState{
            vk::PipelineViewportStateCreateFlags(),
            1, // viewportCount
            {},
            1, // scissorCount
            {}
        };

        vk::PipelineRasterizationStateCreateInfo rasterizer{
            vk::PipelineRasterizationStateCreateFlags(),
            VK_FALSE,
            VK_FALSE,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack
        };

        rasterizer.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisampling{};

        vk::PipelineDepthStencilStateCreateInfo depthStencil{
            vk::PipelineDepthStencilStateCreateFlags(),
            VK_TRUE,
            VK_TRUE,
            vk::CompareOp::eLess,
            VK_FALSE
        };

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlending{
            vk::PipelineColorBlendStateCreateFlags(),
            VK_FALSE,
            vk::LogicOp::eCopy,
            1,
            &colorBlendAttachment
        };

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicState{
            vk::PipelineDynamicStateCreateFlags(),
            dynamicStates
        };

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
            vk::PipelineLayoutCreateFlags(),
            1,
            &descriptorSetLayout
        };

        vk::resultCheck(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout), "failed to create pipeline layout!");

        vk::GraphicsPipelineCreateInfo pipelineInfo{
            vk::PipelineCreateFlags(),
            2,
            shaderStages,
            &vertexInputInfo,
            &inputAssembly,
            {},
            &viewportState,
            &rasterizer,
            &multisampling,
            &depthStencil,
            &colorBlending,
            &dynamicState,
            pipelineLayout,
            renderPass
        };

        vk::resultCheck(device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline), "failed to create graphics pipeline!");

        device.destroyShaderModule(fragShaderModule, nullptr);
        device.destroyShaderModule(vertShaderModule, nullptr);
    }

    void createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<vk::ImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView
            };

            vk::FramebufferCreateInfo framebufferInfo{
                vk::FramebufferCreateFlags(),
                renderPass,
                attachments,
                swapChainExtent.width,
                swapChainExtent.height,
                1
            };

            vk::resultCheck(device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]), "failed to create framebuffer!");
        }
    }

    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        vk::CommandPoolCreateInfo poolInfo{
            vk::CommandPoolCreateFlags(),
            queueFamilyIndices.graphicsFamily.value()
        };

        vk::resultCheck(device.createCommandPool(&poolInfo, nullptr, &commandPool), "failed to create command pool!");
    }

    void createDepthResources() {
        vk::Format depthFormat = findDepthFormat();
        createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
        depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
    }

    vk::Format findDepthFormat() {
        return findSupportedFormat(

            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
    }

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
        for (vk::Format format : candidates) {
            vk::FormatProperties props;
            physicalDevice.getFormatProperties(format, &props);
            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
                return format;
            } 
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    bool hasStencilComponent(vk::Format format) {
        return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
    }

    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
        vk::ImageViewCreateInfo viewInfo{
            vk::ImageViewCreateFlags(),
            image,
            vk::ImageViewType::e2D,
            format
      };

        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vk::ImageView imageView;

        vk::resultCheck(device.createImageView(&viewInfo, nullptr, &imageView), "failed to create texture image view!");
        return imageView;
    }

    void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) {
        vk::ImageCreateInfo imageInfo{
            vk::ImageCreateFlags(),
            vk::ImageType::e2D,
            format,
            { width, height, 1},
            1,
            1,
            vk::SampleCountFlagBits::e1,
            tiling,
            usage,
            vk::SharingMode::eExclusive
      };

        vk::resultCheck(device.createImage(&imageInfo, nullptr, &image), "failed to create image!");

        vk::MemoryRequirements memRequirements;
        device.getImageMemoryRequirements(image, &memRequirements);

        vk::MemoryAllocateInfo allocInfo{
            memRequirements.size,
            findMemoryType(memRequirements.memoryTypeBits, properties)
        };

        vk::resultCheck(device.allocateMemory(&allocInfo, nullptr, &imageMemory), "failed to allocate image memory!");
        device.bindImageMemory(image, imageMemory, 0);
    }

    void createVertexBuffer() {
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        auto data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        device.unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst  | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        device.destroyBuffer(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }

    void createIndexBuffer() {
        vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

        auto data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, indices.data(), (size_t) bufferSize);
        device.unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        device.destroyBuffer(stagingBuffer);
        device.freeMemory(stagingBufferMemory);
    }

    void createUniformBuffers() {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i]);

            uniformBuffersMapped[i] = device.mapMemory(uniformBuffersMemory[i], 0, bufferSize);
        }
    }

    void createDescriptorPool() {
        vk::DescriptorPoolSize poolSize{
            vk::DescriptorType::eUniformBuffer,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
        };

        vk::DescriptorPoolCreateInfo poolInfo{
            vk::DescriptorPoolCreateFlags(),
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
            1,
            &poolSize
        };

        vk::resultCheck(device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool), "failed to create descriptor pool!");
    }

    void createDescriptorSets() {
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        vk::DescriptorSetAllocateInfo allocInfo{
            descriptorPool,
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
            layouts.data()
        };

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        vk::resultCheck(device.allocateDescriptorSets(&allocInfo, descriptorSets.data()), "failed to allocate descriptor sets!");

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::DescriptorBufferInfo bufferInfo{
                uniformBuffers[i],
                0,
                sizeof(UniformBufferObject)
            };

            vk::WriteDescriptorSet descriptorWrite{
                descriptorSets[i],
                0,
                0,
                1,
                vk::DescriptorType::eUniformBuffer,
                {},
                &bufferInfo
            };

            device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }
    }

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
        vk::PhysicalDeviceMemoryProperties memProperties;
        physicalDevice.getMemoryProperties(&memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
        vk::BufferCreateInfo bufferInfo{
            vk::BufferCreateFlags(),
            size,
            usage
        };

        vk::resultCheck(device.createBuffer(&bufferInfo, nullptr, &buffer), "failed to create buffer!");

        vk::MemoryRequirements memRequirements;
        device.getBufferMemoryRequirements(buffer, &memRequirements);

        vk::MemoryAllocateInfo allocInfo{
            memRequirements.size,
            findMemoryType(memRequirements.memoryTypeBits, properties)
        };
        vk::resultCheck(device.allocateMemory(&allocInfo, nullptr, &bufferMemory), "failed to allocate buffer memory!");

        device.bindBufferMemory(buffer, bufferMemory, 0);
    }

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
        vk::CommandBufferAllocateInfo allocInfo{
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            1
        };

        vk::CommandBuffer commandBuffer;
        vk::resultCheck(device.allocateCommandBuffers(&allocInfo, &commandBuffer),"failed to allocate command buffer memory!");

        vk::CommandBufferBeginInfo beginInfo{
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };
        commandBuffer.begin(beginInfo);

        vk::BufferCopy copyRegion{
            0,
            0,
            size
        };
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
        commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vk::resultCheck(graphicsQueue.submit(1, &submitInfo, nullptr), "failed to submit graphics queue");
        graphicsQueue.waitIdle();

        device.freeCommandBuffers(commandPool, 1, &commandBuffer);
    }

    void createCommandBuffers() {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        vk::CommandBufferAllocateInfo allocInfo{
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            (uint32_t) commandBuffers.size()
        };

        vk::resultCheck(device.allocateCommandBuffers(&allocInfo, commandBuffers.data()), "failed to allocate command buffers!");
    }

    void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {
        vk::CommandBufferBeginInfo beginInfo{};

        vk::resultCheck(commandBuffer.begin(&beginInfo), "failed to begin recording command buffer!");

        std::array<vk::ClearValue, 2> clearValues{};
        clearValues[0].setColor({0.0f, 0.0f, 0.0f, 1.0f});
        clearValues[1].setDepthStencil({1.0f, 0});

        vk::RenderPassBeginInfo renderPassInfo{
            renderPass,
            swapChainFramebuffers[imageIndex],
            {{0,0}, swapChainExtent},
            clearValues
        };

        commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        vk::Viewport viewport{
            0.0f, 0.0f,
            static_cast<float>(swapChainExtent.width),
            static_cast<float>(swapChainExtent.height),
            0.0f, 1.0f
        };
        commandBuffer.setViewport(0, 1, &viewport);

        vk::Rect2D scissor{ {0, 0}, swapChainExtent };
        commandBuffer.setScissor(0, 1, &scissor);

        vk::Buffer vertexBuffers[] = {vertexBuffer};
        vk::DeviceSize offsets[] = {0};
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
        commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();
    }

    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        vk::SemaphoreCreateInfo semaphoreInfo{};

        vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled};

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if  (device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != vk::Result::eSuccess ||
                 device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != vk::Result::eSuccess ||
                 device.createFence(&fenceInfo, nullptr, &inFlightFences[i]) != vk::Result::eSuccess) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }


    void drawFrame() {
        vk::resultCheck(device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX), "failed to wait for Fences!");

        uint32_t imageIndex;
        vk::Result result = device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);

        if (result == vk::Result::eErrorOutOfDateKHR) {
            recreateSwapChain();
            return;
        } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(currentFrame);

        vk::resultCheck(device.resetFences(1, &inFlightFences[currentFrame]), "failed to reset Fences!");

        commandBuffers[currentFrame].reset();
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

        vk::SubmitInfo submitInfo{
            1, waitSemaphores, waitStages,
            1, &commandBuffers[currentFrame],
            1, signalSemaphores
        };

        vk::resultCheck(graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]), "failed to submit draw command buffer!");

        vk::PresentInfoKHR presentInfo{
            signalSemaphores,
            swapChain,
            imageIndex,
        };

        result = presentQueue.presentKHR(&presentInfo);

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    static void mouse_button_callback (GLFWwindow* window, int button, int action, int mods) noexcept {
        if (action == GLFW_PRESS)        Input::press_mouse_button(button);
        else if (action == GLFW_RELEASE) Input::release_mouse_button(button);

    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {   
        if (action == GLFW_PRESS || action == GLFW_REPEAT) Input::press_keyboard_key(key);
        else if (action == GLFW_RELEASE)                   Input::release_keyboard_key(key);
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        double x_prev, y_prev;
        glfwGetCursorPos(window.getGLFWwindow(), &x_prev, &y_prev);
        camera.viewer_position += camera.determine_move();
        camera.camera_direction = camera.determine_rotate(x_prev, y_prev);

        UniformBufferObject ubo{};
        ubo.model = glm::mat4(1.0f);
        ubo.view = glm::lookAt(camera.viewer_position, camera.viewer_position + camera.camera_direction, camera.camera_up);
        ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 1000.0f);
        ubo.viewPos = camera.viewer_position;
        ubo.proj[1][1] *= -1;

        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    vk::ShaderModule createShaderModule(const std::vector<char>& code) {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        vk::ShaderModule shaderModule;
        if (device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device) {
        SwapChainSupportDetails details;

        details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
        details.formats = device.getSurfaceFormatsKHR(surface);
        details.presentModes = device.getSurfacePresentModesKHR(surface);

        return details;
    }

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window.getGLFWwindow(), &width, &height);

            vk::Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport() {
        auto availableLayers = vk::enumerateInstanceLayerProperties();

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

glm::vec3 vector_cast_vec3(Geo3D::Vector vec);
glm::vec3 get_normal(const Geo3D::Triangle& triangle);


int vulkan(const std::vector<Geo3D::Triangle>& triangles, std::vector<bool>& status) {
    HelloTriangleApplication app;
    vertices.reserve(triangles.size() * 3);
    indices.reserve(triangles.size() * 3 * 2);

    glm::vec3 red  = {1.0f, 0.0f, 0.0f};
    glm::vec3 blue = {0.0f, 0.0f, 1.0f};
    glm::vec3 color;

    int verticle = 0;
    for (int i = 0; i != triangles.size(); ++i) {
        if (status[i]) {
            color = red;
        }
        else color = blue;

        vertices.push_back({vector_cast_vec3(triangles[i].v0_), color, get_normal(triangles[i])});
        vertices.push_back({vector_cast_vec3(triangles[i].v1_), color, get_normal(triangles[i])});
        vertices.push_back({vector_cast_vec3(triangles[i].v2_), color, get_normal(triangles[i])});
    }


    for (int i = 0; i != triangles.size() * 3; ++i) {
        indices.push_back(i);
    }

    for (int i = triangles.size() * 3 - 1; i != -1; --i) {
        indices.push_back(i);
    }

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

glm::vec3 vector_cast_vec3(Geo3D::Vector vec) {
    glm::vec3 tmp;
    tmp.x = vec.x_; tmp.y = vec.y_; tmp.z = vec.z_;

    return tmp;
}

glm::vec3 get_normal(const Geo3D::Triangle& triangle) {
    glm::vec3 side10, side20;

    side10.x = triangle.v1_.x_ - triangle.v0_.x_; 
    side10.y = triangle.v1_.y_ - triangle.v0_.y_;
    side10.z = triangle.v1_.z_ - triangle.v0_.z_;

    side20.x = triangle.v2_.x_ - triangle.v0_.x_;
    side20.y = triangle.v2_.y_ - triangle.v0_.y_;
    side20.z = triangle.v2_.z_ - triangle.v0_.z_;

    return glm::normalize(glm::cross(side10, side20));
}