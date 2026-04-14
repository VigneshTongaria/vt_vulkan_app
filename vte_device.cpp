#include "vte_device.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>

VteDevice::VteDevice(std::string name) : name(name)
{
    CreateInstance();
}

VteDevice::~VteDevice()
{
    vkDestroyInstance(vkinstance,nullptr);
}

void VteDevice::CreateInstance()
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

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

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
    if(enableValidationLayer)
    {
        createInfo.enabledLayerCount = static_cast<u_int32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
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