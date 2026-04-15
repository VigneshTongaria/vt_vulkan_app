#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <optional>
#include "vte_window.hpp"

struct QueueFamilyIndices
{
    std::optional<u_int32_t> graphicsFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value();
    }
};

class VteDevice
{

public:
    VteDevice(std::string name, vte::Vtewindow &window);
    ~VteDevice();

    vte::Vtewindow vteWindow;

private:
    
    std::string name;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    #ifdef NDEBUG
       const bool enableValidationLayer = true;
    #else
       const bool enableValidationLayer = false;
     #endif

    VkInstance vkinstance;
    VkDebugUtilsMessengerEXT debugMessenger;

    // Vulkan instance
    void createInstance();
    bool checkValidationLayerSupport();
    void setupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    VkResult createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void destroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator);

    VkSurfaceKHR surface;
    void createSurface();

    // Device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkDevice device;
    VkQueue graphicsQueue;

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void createLogicalDevice();
};


