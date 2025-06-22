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
  
  App::App(const int width, const int height, std::string windowname) {
    windowInfo.width = width;
    windowInfo.height = height;
    windowInfo.windowName = windowname;

    initWindow();
    initVulkan();
  }

  App::~App() {
    cleanup();
  }
  
  void App::run() {
    mainLoop();
  }

  void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void App::initVulkan() {
    createVKInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
  }

  App::QueueFamilyIndices App::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }
      
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vkSurface, &presentSupport);

      if (presentSupport) {
        indices.presentFamily = i;
      }

      if (indices.isComplete()) {
        break;
      }

      i++;
    }
    return indices;
  }

  int App::rateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices indices = findQueueFamilies(device);
    bool requiredExtensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;

    if (requiredExtensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
      swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    if (!deviceFeatures.geometryShader || !indices.isComplete() || !requiredExtensionsSupported || !swapChainAdequate) {
      return 0;
    }

    int score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      score += 32768;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
  }

  void App::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0) {
      throw std::runtime_error("Failed to find GPU's with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice, std::greater<int>> candidates;

    for (const auto& device : devices) {
      int score = rateDeviceSuitability(device);
      candidates.insert(std::make_pair(score, device));
    }

    for (const auto& candidate : candidates) {
      if (candidate.first > 0) {
        vkPhysicalDevice = candidate.second;

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
        std::cout << "Using device: " << deviceProperties.deviceName << "\nWith score: " << candidate.first << '\n';
        break;
      }
    }

    if (vkPhysicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("Failed to find a suitable GPU!");
    }

  }

  bool App::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for (const char* layerName : vkValidationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  App::SwapChainSupportDetails App::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkSurface, &details.capabilities);


    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, nullptr);

    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, details.presentModes.data());
    }


    return details;
  }

  VkSurfaceFormatKHR App::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  VkPresentModeKHR App::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D App::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      
      VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
      };

      actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
      return actualExtent;
    }
  }

  void App::createGraphicsPipeline() {

  }

  void App::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;
      createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &vkSwapChain) != VK_SUCCESS) { 
      throw std::runtime_error("failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, nullptr);
    vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &imageCount, vkSwapChainImages.data());

    vkSwapChainImageFormat = surfaceFormat.format;
    vkSwapChainExtent = extent;
  }

  void App::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures{};
    

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(vkDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = vkDeviceExtensions.data();


    if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationLayers.size());
      createInfo.ppEnabledLayerNames = vkValidationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(vkDevice, indices.graphicsFamily.value(), 0, &vkGraphicsQueue);
    vkGetDeviceQueue(vkDevice, indices.presentFamily.value(),  0, &vkPresentQueue);

  }

  void App::createSurface() {
    if (glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  }

  void App::createImageViews() {
    vkSwapChainImageViews.resize(vkSwapChainImages.size());

    for (size_t i = 0; i < vkSwapChainImages.size(); i++) {
      VkImageViewCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = vkSwapChainImages[i];
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = vkSwapChainImageFormat;

      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(vkDevice, &createInfo, nullptr, &vkSwapChainImageViews[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image views!");
      }
    }
  }

  bool App::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(vkDeviceExtensions.begin(), vkDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();

  }

  void App::createVKInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("Vaildation layers requested but not avaiable!");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = windowInfo.windowName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(vkValidationLayers.size());
      createInfo.ppEnabledLayerNames = vkValidationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }
    createInfo.pNext = nullptr;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    //createInfo.enabledLayerCount = 0;



    // attempt to create instance and throw if fail
    if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create vk instance!");
    }


    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "Avaiable Extensions:\n";
    for (const auto& extension : extensions) {
      std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << extensionCount << " Avaiable Extensions.\n";
  }

  void App::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(windowInfo.width, windowInfo.height, windowInfo.windowName.c_str(), nullptr, nullptr);
  }

  void App::cleanup() {
    for (auto imageView : vkSwapChainImageViews) {
      vkDestroyImageView(vkDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    
    glfwDestroyWindow(window);
    glfwTerminate();
  }
} // namespace malodee
