#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
#include <string>
#include <vector>
#include <optional>

namespace Malodee {

  class App {
    public:

    struct WindowInfo
    {
      uint32_t width;
      uint32_t height;
      std::string windowName;
    };

    App(const int width, const int height, std::string windowname);
    ~App();

    void run();

    private:
    void initWindow();
    void initVulkan();
    
    //vulkan
    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };
    struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    };
    
    void createVKInstance();
    bool checkValidationLayerSupport();
    void pickPhysicalDevice();
    int  rateDeviceSuitability(VkPhysicalDevice device);
    void createLogicalDevice();
    void createSurface();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    //not vulkan

    void mainLoop();

    void cleanup();

    GLFWwindow* window;
    WindowInfo  windowInfo;

    //vulkan
    VkInstance vkInstance;
    VkDevice vkDevice;
    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;
    VkSurfaceKHR vkSurface;
    VkSwapchainKHR vkSwapChain;
    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
    VkFormat vkSwapChainImageFormat;
    VkExtent2D vkSwapChainExtent;
    std::vector<VkImage> vkSwapChainImages;
    std::vector<VkImageView> vkSwapChainImageViews;
    const std::vector<const char*> vkValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> vkDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    #ifdef NDEBUG 
      const bool enableValidationLayers = false;
    #else
      const bool enableValidationLayers = true;
    #endif
    
  };
}