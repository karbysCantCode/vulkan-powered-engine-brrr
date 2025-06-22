#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


#include "vulkan_core_window.hpp"
#include "vulkan_core_instance.hpp"
#include "vulkan_core_device.hpp"
#include "vulkan_core_swapchain.hpp"


//std
#include <string>
#include <vector>
#include <optional>

namespace Malodee {

  class App {
    public:

    App(const int width, const int height, std::string windowname);
    ~App();

    void run();

    private:
    void mainLoop();
    void cleanup();


    Vulkan::Window vkWindow;
    Vulkan::Instance vkInstance;
    Vulkan::Device vkDevice;
    Vulkan::SwapChain vkSwapChain;
  };
}