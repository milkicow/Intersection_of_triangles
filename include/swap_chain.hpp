#include "device.hpp"
#include "window.hpp"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <string>
#include <vector>

namespace vulkan_engine {

class SwapChain {
public:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Window & window, Device & device) : window_(window), device_(device) {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    };

    //~SwapChain();
    void cleanup();
    
    vk::SwapchainKHR getSwapChain() const { return swapChain_; }
    vk::RenderPass getRenderPass() const { return renderPass_; }
    std::vector<vk::Framebuffer> getFramebuffers() const { return swapChainFramebuffers_; }
    vk::Extent2D getExtent() const { return swapChainExtent_; }
    
    std::vector<vk::Semaphore> getImageAvailableSemaphores() const { return imageAvailableSemaphores_; }
    std::vector<vk::Semaphore> getRenderFinishedSemaphores() const { return renderFinishedSemaphores_; }
    std::vector<vk::Fence>     getInFlightFences() const           { return inFlightFences_; }
    
    void recreate();

    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;

private:
  
    Window & window_;
    Device & device_;
    vk::Extent2D swapChainExtent_;

    vk::SwapchainKHR swapChain_;
    std::vector<vk::Image> swapChainImages_;
    vk::Format swapChainImageFormat_;
    std::vector<vk::ImageView> swapChainImageViews_;
    std::vector<vk::Framebuffer> swapChainFramebuffers_;
    vk::RenderPass renderPass_;

    vk::Image depthImage_;
    vk::DeviceMemory depthImageMemory_;
    vk::ImageView depthImageView_;

    std::vector<vk::Semaphore> imageAvailableSemaphores_;
    std::vector<vk::Semaphore> renderFinishedSemaphores_;
    std::vector<vk::Fence> inFlightFences_;

    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDepthResources();
    void createFramebuffers();
    void createSyncObjects();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR   chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D         chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    vk::Format findDepthFormat();
};
}