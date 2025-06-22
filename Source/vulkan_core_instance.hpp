#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan_core_window.hpp"

//std
#include <vector>


namespace Malodee {
namespace Vulkan {

  class Instance {
    public:
    
    Instance(Window* window);
    ~Instance();
    
    void createInstance();

    VkInstance instance;
    Window* pWindow;

    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    #ifdef NDEBUG 
      const bool enableValidationLayers = false;
    #else
      const bool enableValidationLayers = true;
    #endif

    private:
    bool checkValidationLayerSupport();
  };
}
}