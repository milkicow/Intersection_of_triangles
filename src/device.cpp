#include "device.hpp"
#include "gtest/gtest.h"
#include <iostream>

namespace vulkan_engine {

void CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger) {
    vk::DispatchLoaderDynamic dynamic_loader{ instance, vkGetInstanceProcAddr };
    vk::resultCheck(instance.createDebugUtilsMessengerEXT(pCreateInfo, pAllocator, pDebugMessenger, dynamic_loader), "failed to create debug utils messenger");
}

void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator) {
    vk::DispatchLoaderDynamic dynamic_loader{ instance, vkGetInstanceProcAddr };
    instance.destroyDebugUtilsMessengerEXT(debugMessenger, pAllocator, dynamic_loader);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


Device::~Device() {
    device_.destroyCommandPool(commandPool_, nullptr);

    device_.destroy();

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
    }

    instance_.destroySurfaceKHR(surface_, nullptr);
    instance_.destroy();
}

void Device::createInstance() {
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
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    vk::resultCheck(vk::createInstance(&createInfo, nullptr, &instance_), "failed to create instance!");
}

void Device::setupDebugMessenger() {
     if (!enableValidationLayers) return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_);
}

void Device::pickPhysicalDevice() {
    auto devices = instance_.enumeratePhysicalDevices();

    auto iter = std::find_if(devices.begin(), devices.end(), [&](auto& device) {
        return isDeviceSuitable(device);
    });

    if (iter != devices.end()) physicalDevice_ = *iter;
    else throw std::runtime_error("failed to find a suitable GPU!");
}

void Device::createSurface() { window_.createWindowSurface(instance_, reinterpret_cast<VkSurfaceKHR *>(&surface_)); }

void Device::createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority);
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::PhysicalDeviceFeatures deviceFeatures{};
        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, validationLayers, deviceExtensions, &deviceFeatures);

        vk::resultCheck(physicalDevice_.createDevice(&createInfo, nullptr, &device_), "failed to create logical device!");
        device_.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue_);
        device_.getQueue(indices.presentFamily.value(), 0, &presentQueue_);
}

void Device::createCommandpool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice_);

        vk::CommandPoolCreateInfo poolInfo{
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            queueFamilyIndices.graphicsFamily.value()
        };

        vk::resultCheck(device_.createCommandPool(&poolInfo, nullptr, &commandPool_), "failed to create command pool!");
};




bool Device::isDeviceSuitable(vk::PhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Device::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    auto availableExtensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Device::findQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;
    auto queueFamilies = device.getQueueFamilyProperties();

    int i = 0;

    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        vk::resultCheck(device.getSurfaceSupportKHR(i, surface_, &presentSupport), "failed to get surface support KHR");

        if (presentSupport) indices.presentFamily = i;
        if (indices.isComplete()) break;
        ++i;
    }
    return indices;
}

SwapChainSupportDetails Device::querySwapChainSupport(vk::PhysicalDevice device) {
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface_);
    details.formats = device.getSurfaceFormatsKHR(surface_);
    details.presentModes = device.getSurfacePresentModesKHR(surface_);

    return details;
}


bool Device::checkValidationLayerSupport() {
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

std::vector<const char*> Device::getRequiredExtensions() {
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

void Device::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(),
             vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
             vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
             debugCallback);
}

uint32_t Device::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties;
    physicalDevice_.getMemoryProperties(&memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

vk::Format Device::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props;
        physicalDevice_.getFormatProperties(format, &props);
        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } 
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

void Device::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
    vk::BufferCreateInfo bufferInfo{
        vk::BufferCreateFlags(),
        size,
        usage
    };

    vk::resultCheck(device_.createBuffer(&bufferInfo, nullptr, &buffer), "failed to create buffer!");

    vk::MemoryRequirements memRequirements;
    device_.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, properties)
    };
    vk::resultCheck(device_.allocateMemory(&allocInfo, nullptr, &bufferMemory), "failed to allocate buffer memory!");

    device_.bindBufferMemory(buffer, bufferMemory, 0);
}

void Device::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{
        commandPool_,
        vk::CommandBufferLevel::ePrimary,
        1
    };

    vk::CommandBuffer commandBuffer;
    vk::resultCheck(device_.allocateCommandBuffers(&allocInfo, &commandBuffer),"failed to allocate command buffer memory!");

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

    vk::resultCheck(graphicsQueue_.submit(1, &submitInfo, nullptr), "failed to submit graphics queue");
    graphicsQueue_.waitIdle();

    device_.freeCommandBuffers(commandPool_, 1, &commandBuffer);
}

void Device::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image& image, vk::DeviceMemory& imageMemory) {
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

    vk::resultCheck(device_.createImage(&imageInfo, nullptr, &image), "failed to create image!");

    vk::MemoryRequirements memRequirements;
    device_.getImageMemoryRequirements(image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{
        memRequirements.size,
        findMemoryType(memRequirements.memoryTypeBits, properties)
    };

    vk::resultCheck(device_.allocateMemory(&allocInfo, nullptr, &imageMemory), "failed to allocate image memory!");
    device_.bindImageMemory(image, imageMemory, 0);
}

}