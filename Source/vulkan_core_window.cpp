#include "vulkan_core_window.hpp"

//std
#include <stdexcept>

namespace Malodee {
namespace Vulkan {

  Window::Window(uint32_t width, uint32_t height, std::string windowName, VkInstance* vkInstance) {
    windowInfo.width = width;
    windowInfo.height = height;
    windowInfo.windowName = windowName;
    
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(windowInfo.width, windowInfo.height, windowInfo.windowName.c_str(), nullptr, nullptr);

    if (vkInstance != nullptr) {
      createSurface();
    }
  }

  Window::~Window() {
    glfwDestroyWindow(pWindow);
  }

  void Window::DestroySurface() {
    vkDestroySurfaceKHR(*pVkInstance, surface, nullptr);
  }

  void Window::createSurface() {
    if (glfwCreateWindowSurface(*pVkInstance, pWindow, nullptr, &surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  }

  void Window::setVkInstance(VkInstance* instance) {
    pVkInstance = instance;
  }
}
}