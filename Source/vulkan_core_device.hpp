#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_core_window.hpp"
#include "vulkan_core_instance.hpp"
#include "vulkan_core_swapchain.hpp"

//std
#include <vector>
#include <optional>

namespace Malodee {
namespace Vulkan {

  class Device {
    public:
    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
      }
    };

    Device(Instance *instance, Window *window);
    ~Device();

    void initDevice(Window *window);
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice aPhysicalDevice);

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    private:

    void pickPhysicalDevice();
    void createLogicalDevice();
    int  rateDeviceSuitability(VkPhysicalDevice aPhysicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice);
    
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    Window* pWindow;
    Instance* pInstance;
  };
}
}