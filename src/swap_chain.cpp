#include "swap_chain.hpp"
#include "device.hpp"


namespace vulkan_engine {

//SwapChain::~SwapChain() {
//    device_.getDevice().destroyRenderPass(renderPass_, nullptr);
//
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        device_.getDevice().destroySemaphore(renderFinishedSemaphores_[i], nullptr);
//        device_.getDevice().destroySemaphore(imageAvailableSemaphores_[i], nullptr);
//        device_.getDevice().destroyFence(inFlightFences_[i], nullptr);
//    }
//
//
//for (auto imageView : swapChainImageViews_) {
//    vkDestroyImageView(device_.getDevice(), imageView, nullptr);
//  }
//
//}


void SwapChain::cleanup() {
        device_.getDevice().destroyImageView(depthImageView_, nullptr);

        device_.getDevice().destroyImage(depthImage_, nullptr);
        device_.getDevice().freeMemory(depthImageMemory_, nullptr);

        for (size_t i = 0; i < swapChainFramebuffers_.size(); i++) {
            device_.getDevice().destroyFramebuffer(swapChainFramebuffers_[i], nullptr);
        }

        for (size_t i = 0; i < swapChainImageViews_.size(); i++) {
            device_.getDevice().destroyImageView(swapChainImageViews_[i], nullptr);
        }
        device_.getDevice().destroySwapchainKHR(swapChain_, nullptr);
}


void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = device_.querySwapChainSupport(device_.getPhysicalDevice());

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo(
        vk::SwapchainCreateFlagsKHR(), device_.getSurface(), imageCount, surfaceFormat.format,
        surfaceFormat.colorSpace, extent, 1, vk::ImageUsageFlagBits::eColorAttachment
        );

    QueueFamilyIndices indices = device_.findQueueFamilies(device_.getPhysicalDevice());
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

    vk::resultCheck(device_.getDevice().createSwapchainKHR(&createInfo, nullptr, &swapChain_), "failed to create swap chain!");

    swapChainImages_ = device_.getDevice().getSwapchainImagesKHR(swapChain_);
    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent;
}

void SwapChain::createImageViews() {
    swapChainImageViews_.resize(swapChainImages_.size());

    for (uint32_t i = 0; i < swapChainImages_.size(); i++) {
        swapChainImageViews_[i] = createImageView(swapChainImages_[i], swapChainImageFormat_, vk::ImageAspectFlagBits::eColor);
    }
}

void SwapChain::createRenderPass() {
    vk::AttachmentDescription colorAttachment(
        vk::AttachmentDescriptionFlags(),
        swapChainImageFormat_,
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
      vk::ImageLayout::eDepthStencilAttachmentOptimal
  );

    vk::AttachmentReference colorAttachmentRef{ 0, vk::ImageLayout::eColorAttachmentOptimal };

    vk::AttachmentReference depthAttachmentRef{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

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


    vk::resultCheck(device_.getDevice().createRenderPass(&renderPassInfo, nullptr, &renderPass_), "failed to create render pass!");
}

void SwapChain::createDepthResources() {
    vk::Format depthFormat = findDepthFormat();
    device_.createImage(swapChainExtent_.width, swapChainExtent_.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage_, depthImageMemory_);
    depthImageView_ = createImageView(depthImage_, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers_.resize(swapChainImageViews_.size());

    for (size_t i = 0; i < swapChainImageViews_.size(); i++) {
        std::array<vk::ImageView, 2> attachments = {
            swapChainImageViews_[i],
            depthImageView_
        };

        vk::FramebufferCreateInfo framebufferInfo{
            vk::FramebufferCreateFlags(),
            renderPass_,
            attachments,
            swapChainExtent_.width,
            swapChainExtent_.height,
            1
        };

        vk::resultCheck(device_.getDevice().createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers_[i]), "failed to create framebuffer!");
    }
}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if  (device_.getDevice().createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != vk::Result::eSuccess ||
             device_.getDevice().createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != vk::Result::eSuccess ||
             device_.getDevice().createFence(&fenceInfo, nullptr, &inFlightFences_[i]) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

vk::Format SwapChain::findDepthFormat() {
    return device_.findSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

vk::ImageView SwapChain::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
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

    vk::resultCheck(device_.getDevice().createImageView(&viewInfo, nullptr, &imageView), "failed to create texture image view!");
    return imageView;
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window_.getGLFWwindow(), &width, &height);

        vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


void SwapChain::recreate() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window_.getGLFWwindow(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window_.getGLFWwindow(), &width, &height);
        glfwWaitEvents();
    }
    device_.getDevice().waitIdle();

    SwapChain::~SwapChain();

    createSwapChain();
    createImageViews();
    createDepthResources();
    createFramebuffers();
}


}