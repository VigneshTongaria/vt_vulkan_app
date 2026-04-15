#include "vte_device.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

VteDevice::VteDevice(std::string name) : name(name)
{
    createInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
}

VteDevice::~VteDevice()
{
    vkDestroyInstance(vkinstance,nullptr);
    vkDestroyDevice(device,nullptr);

    if(enableValidationLayer)
    {
        destroyDebugUtilsMessengerEXT(vkinstance,debugMessenger,nullptr);
    }
}

void VteDevice::createInstance()
{
    if(enableValidationLayer && !checkValidationLayerSupport())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "vt engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> glfwExtensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = static_cast<u_int32_t>(glfwExtensions.size());
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

    if(enableValidationLayer)
    {
        createInfo.enabledLayerCount = static_cast<u_int32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(createDebugInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&createDebugInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo,nullptr,&vkinstance) != VK_SUCCESS)
    {
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

bool VteDevice::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount,nullptr);

    std::vector<VkLayerProperties> availaibleLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount,availaibleLayers.data());

    for(const char* layerName : validationLayers)
    {
        bool layerFound = false;
        for(const auto& currentLayer : availaibleLayers)
        {
            if(strcmp(layerName,currentLayer.layerName))
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
         return false;
    }

    return true;
}

std::vector<const char*> VteDevice::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,glfwExtensions + glfwExtensionCount);

    if(enableValidationLayer)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VteDevice::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
)
{
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VteDevice::setupDebugMessenger()
{
    if (!enableValidationLayer)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{};
    populateDebugMessengerCreateInfo(createDebugInfo);

    if(createDebugUtilsMessengerEXT(vkinstance,&createDebugInfo,nullptr,&debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult VteDevice::createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");

    if(func != nullptr)
    {
        return func(instance,pCreateInfo,pAllocator,pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VteDevice::destroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void VteDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createDebugInfo)
{
    createDebugInfo = {};
    createDebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createDebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createDebugInfo.pfnUserCallback = debugCallback;
    createDebugInfo.pUserData = nullptr;
}

void VteDevice::pickPhysicalDevice()
{
    u_int32_t devicesCount = 0;
    vkEnumeratePhysicalDevices(vkinstance,&devicesCount,nullptr);

    if(devicesCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with vulkan support");
    }

    std::vector<VkPhysicalDevice> physicalDevices(devicesCount);

    vkEnumeratePhysicalDevices(vkinstance,&devicesCount,physicalDevices.data());

    for(auto& dev : physicalDevices)
    {
        if(isDeviceSuitable(dev))
        {
            physicalDevice = dev;
            break;
        }
    }
}

bool VteDevice::isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device,&deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device,&deviceFeatures);

    QueueFamilyIndices queueIndices = findQueueFamilies(device);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        && deviceFeatures.geometryShader
        && queueIndices.isComplete();
}

QueueFamilyIndices VteDevice::findQueueFamilies(VkPhysicalDevice device)
{
   QueueFamilyIndices indices;

   uint32_t queueFamilyCount = 0;
   vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,nullptr);
   
   std::vector<VkQueueFamilyProperties> queueFamilies;
   vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,queueFamilies.data());
   u_int32_t i = 0;

   for (const auto &family : queueFamilies)
   {
       if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
       {
           indices.graphicsFamily = i;
           break;
       }
       i++;
   }

   return indices;
}

void VteDevice::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if(enableValidationLayer)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<u_int32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice,&deviceCreateInfo,nullptr,&device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device,indices.graphicsFamily.value(),0,&graphicsQueue);
}