#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//std
#include <string>



namespace Malodee {
namespace Vulkan {
  class Window {
    public:
    struct WindowInfo
    {
      uint32_t width;
      uint32_t height;
      std::string windowName;
    };

    Window(uint32_t width, uint32_t height, std::string windowName, VkInstance* vkInstance = nullptr);
    ~Window();

    void createSurface();
    void DestroySurface();

    void setVkInstance(VkInstance* instance);

    VkSurfaceKHR surface;
    GLFWwindow* pWindow;
    VkInstance* pVkInstance;
    WindowInfo  windowInfo;
  };
}
}