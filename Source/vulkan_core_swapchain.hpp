#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



#include "vulkan_core_device.hpp"
#include "vulkan_core_window.hpp"

//std
#include <vector>

namespace Malodee {
namespace Vulkan {

  class SwapChain {
    public:

    SwapChain(Device *device, Window *window);
    ~SwapChain();

    void createSwapChain();
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    Device* pDevice;
    Window* pWindow;
    VkSwapchainKHR swapchain;

    VkFormat vkSwapChainImageFormat;
    VkExtent2D vkSwapChainExtent;
    std::vector<VkImage> vkSwapChainImages;
    std::vector<VkImageView> vkSwapChainImageViews;
  };
}
}