#include "vte_device.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <vulkan/vulkan_core.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3native.h>

VteDevice::VteDevice(std::string name, vte::Vtewindow &window)
    : name(name), vteWindow(window) {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  CreateSwapChain();
  CreateImageViews();
}

VteDevice::~VteDevice() {

}

void VteDevice::cleanDevice()
{
    for (auto imageView : swapChainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    if (enableValidationLayer) {
      destroyDebugUtilsMessengerEXT(vkinstance, debugMessenger, nullptr);
    }

    vkDestroyInstance(vkinstance, nullptr);
}

VkDevice &VteDevice::getVkDevice() { return device; }

void VteDevice::createInstance() {
  if (enableValidationLayer && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = name.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "vt engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};

  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  std::vector<const char *> glfwExtensions = getRequiredExtensions();

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(glfwExtensions.size());
  createInfo.ppEnabledExtensionNames = glfwExtensions.data();

  // Fall back if current extensions fails
  // std::vector<const char *> requiredExtensions;

  // for (uint32_t i = 0; i < glfwExtensionCount; i++)
  // {
  //     requiredExtensions.emplace_back(glfwExtensions[i]);
  // }

  // requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  // createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  // createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  // createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  // Add validation layers if enabled

  VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{};

  if (enableValidationLayer) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(createDebugInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&createDebugInfo;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &vkinstance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create vk instance");
  }

  // Finding all supported extensions
  // uint32_t extensionsCount = 0;
  // vkEnumerateInstanceExtensionProperties(nullptr,&extensionsCount,nullptr);
  // std::vector<VkExtensionProperties> extensions(extensionsCount);
  // vkEnumerateInstanceExtensionProperties(nullptr,&extensionsCount,extensions.data());

  // std::cout << "available extensions:"<<extensions.size()<<"\n";

  // for (const auto &extension : extensions)
  // {
  //     std::cout << '\t' << extension.extensionName << '\n';
  // }
}

bool VteDevice::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availaibleLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availaibleLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;
    for (const auto &currentLayer : availaibleLayers) {
      if (strcmp(layerName, currentLayer.layerName)) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
      return false;
  }

  return true;
}

std::vector<const char *> VteDevice::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enableValidationLayer) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  for (const auto &ex : extensions) {
    std::cout << "Extension added : " << ex << std::endl;
  }

  return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VteDevice::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void VteDevice::setupDebugMessenger() {
  if (!enableValidationLayer)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{};
  populateDebugMessengerCreateInfo(createDebugInfo);

  if (createDebugUtilsMessengerEXT(vkinstance, &createDebugInfo, nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

VkResult VteDevice::createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");

  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void VteDevice::destroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void VteDevice::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createDebugInfo) {
  createDebugInfo = {};
  createDebugInfo.sType =
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createDebugInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createDebugInfo.pfnUserCallback = debugCallback;
  createDebugInfo.pUserData = nullptr;
}

void VteDevice::createSurface() {
  VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.display = glfwGetWaylandDisplay();
  surfaceCreateInfo.surface = glfwGetWaylandWindow(vteWindow.window);

  if (vkCreateWaylandSurfaceKHR(vkinstance, &surfaceCreateInfo, nullptr,
                                &surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void VteDevice::pickPhysicalDevice() {
  uint32_t devicesCount = 0;
  vkEnumeratePhysicalDevices(vkinstance, &devicesCount, nullptr);

  if (devicesCount == 0) {
    throw std::runtime_error("failed to find GPUs with vulkan support");
  }

  std::vector<VkPhysicalDevice> physicalDevices(devicesCount);

  vkEnumeratePhysicalDevices(vkinstance, &devicesCount, physicalDevices.data());

  for (auto &dev : physicalDevices) {
    if (isDeviceSuitable(dev)) {
      physicalDevice = dev;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find suitable physical device");
  }
  else {
      queueFamilies = findQueueFamilies(physicalDevice);
  }
}

bool VteDevice::isDeviceSuitable(VkPhysicalDevice device) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  QueueFamilyIndices queueIndices = findQueueFamilies(device);
  bool extensionSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;

  if (extensionSupported) {
    SwapChainSupportDetails swapChainSupportDetails =
        querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
                        !swapChainSupportDetails.formats.empty();
  }

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader && queueIndices.isComplete() &&
         extensionSupported && swapChainAdequate;
}

QueueFamilyIndices VteDevice::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  uint32_t i = 0;
  VkBool32 presentSupport = false;

  for (const auto &family : queueFamilies) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

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

void VteDevice::createLogicalDevice() {

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  std::set<uint32_t> uniqueQueueFamilies = {queueFamilies.graphicsFamily.value(),
                                            queueFamilies.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;

    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayer) {
    deviceCreateInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    deviceCreateInfo.enabledLayerCount = 0;
  }

  // Specify to vulkan we are not looking for mobile/other application
  deviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
  deviceFeatures.textureCompressionETC2 = VK_FALSE;

  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device, queueFamilies.graphicsFamily.value(), 0, &graphicsQueue);
  vkGetDeviceQueue(device, queueFamilies.presentFamily.value(), 0, &presentQueue);
}

void VteDevice::CreateSwapChain() {
  SwapChainSupportDetails swapChainSupportDetails =
      querySwapChainSupport(physicalDevice);
  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupportDetails.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities);

  uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

  if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupportDetails.capabilities.maxImageCount) {
    imageCount = swapChainSupportDetails.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapChainCreateInfo{};

  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = surface;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = extent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamiliesArr[] = {queueFamilies.graphicsFamily.value(),
                              queueFamilies.presentFamily.value()};

  if (queueFamilies.graphicsFamily != queueFamilies.presentFamily) {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = queueFamiliesArr;
  } else {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  }

  swapChainCreateInfo.preTransform =
      swapChainSupportDetails.capabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());
  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;
}

void VteDevice::CreateImageViews() {
  swapChainImageViews.resize(swapChainImages.size());

  for (size_t i = 0; i < swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr,
                          &swapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

bool VteDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availaibleExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availaibleExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto &extensions : availaibleExtensions) {
    requiredExtensions.erase(extensions.extensionName);
  }

  return requiredExtensions.empty();
}

SwapChainSupportDetails
VteDevice::querySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details{};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR VteDevice::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR VteDevice::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VteDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;

    glfwGetFramebufferSize(vteWindow.window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}
