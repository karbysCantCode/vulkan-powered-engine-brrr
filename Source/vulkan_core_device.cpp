#include "vulkan_core_device.hpp"

//std
#include <vector>
#include <stdexcept>
#include <map>
#include <iostream>
#include <set>

namespace Malodee {
namespace Vulkan {

  Device::Device(Instance *instance, Window *window = nullptr) {
    if (window != nullptr) {
      pWindow = window;
    }
  }

  void Device::initDevice(Window *window = nullptr) {
    if (window != nullptr) {
      pWindow = window;
    }

    pickPhysicalDevice();
  }
    
  void Device::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

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

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();


    if (pInstance->enableValidationLayers) {
      createInfo.enabledLayerCount = static_cast<uint32_t>(pInstance->validationLayers.size());
      createInfo.ppEnabledLayerNames = pInstance->validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(),  0, &presentQueue);

  }

  void Device::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(pInstance->instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
      throw std::runtime_error("Failed to find GPU's with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(pInstance->instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice, std::greater<int>> candidates;

    for (const auto& device : devices) {
      int score = rateDeviceSuitability(device);
      candidates.insert(std::make_pair(score, device));
    }

    for (const auto& candidate : candidates) {
      if (candidate.first > 0) {
        physicalDevice = candidate.second;

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        std::cout << "Using device: " << deviceProperties.deviceName << "\nWith score: " << candidate.first << '\n';
        break;
      }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("Failed to find a suitable GPU!");
    }
  }

  int Device::rateDeviceSuitability(VkPhysicalDevice aPhysicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(aPhysicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(aPhysicalDevice, &deviceFeatures);

    QueueFamilyIndices indices = findQueueFamilies(aPhysicalDevice);
    bool requiredExtensionsSupported = checkDeviceExtensionSupport(aPhysicalDevice);
    bool swapChainAdequate = false;

    if (requiredExtensionsSupported) {
      SwapChain::SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(aPhysicalDevice, &pWindow->surface);
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

  bool Device::checkDeviceExtensionSupport(VkPhysicalDevice aPhysicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(aPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  Device::QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice aPhysicalDevice) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevice, &queueFamilyCount, queueFamilies.data());


    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }
      
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(aPhysicalDevice, i, pWindow->surface, &presentSupport);

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
}
}