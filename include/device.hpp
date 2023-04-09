#pragma once

#include "window.hpp"
#include <vector>
#include <string>
#include <optional>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>


namespace vulkan_engine {

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device {
public:

    Device(Window &window) : window_(window) {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandpool();
    }
    ~Device();

    Device(const Device&)  = delete;
    Device & operator=(const Device&) = delete;
    Device(const Device&&) = delete;
    Device & operator=(const Device&&) = delete;

    const vk::CommandPool& getCommandPool()       const & { return commandPool_; }
    const vk::Device& getDevice()                 const & { return device_; }
    const vk::SurfaceKHR& getSurface()            const & { return surface_; }
    const vk::Queue& getGraphicsQueue()           const & { return graphicsQueue_; }
    const vk::Queue& getPresentQueue()            const & { return presentQueue_; }
    const vk::PhysicalDevice& getPhysicalDevice() const & { return physicalDevice_; }
    
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
    void createImage(uint32_t width, uint32_t height,
                     vk::Format format, vk::ImageTiling tiling,
                     vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties,
                     vk::Image& image,
                     vk::DeviceMemory& imageMemory);


    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
    

private:

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandpool();


    Window &window_;
    vk::Instance instance_;
    vk::Device device_;
    vk::CommandPool commandPool_;
    vk::SurfaceKHR surface_;
    vk::Queue graphicsQueue_;
    vk::Queue presentQueue_;

    vk::DebugUtilsMessengerEXT debugMessenger_;
    vk::PhysicalDevice physicalDevice_ = nullptr;
    
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
    bool isDeviceSuitable(vk::PhysicalDevice device);
    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
};

}
