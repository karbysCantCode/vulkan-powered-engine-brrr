#include "app.hpp"

//std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <cstdint>
#include <limits>
#include <algorithm>

namespace Malodee {
  
  App::App(const int width, const int height, std::string windowname)
  : vkWindow(Vulkan::Window(width, height, windowname)),
    vkInstance(Vulkan::Instance(&vkWindow)),
    vkDevice(Vulkan::Device(&vkInstance, &vkWindow)),
    vkSwapChain(Vulkan::SwapChain(&vkDevice, &vkWindow))
   {
  }

  App::~App() {
    cleanup();
  }
  
  void App::run() {
    mainLoop();
  }

  void App::mainLoop() {
    while (!glfwWindowShouldClose(vkWindow.pWindow)) {
      glfwPollEvents();
    }
  }

  void App::cleanup() {
    vkSwapChain.~SwapChain();
    vkDevice.~Device();
    vkWindow.DestroySurface();
    vkInstance.~Instance();
    vkWindow.~Window();
    
    glfwTerminate();
  }
} // namespace malodee
