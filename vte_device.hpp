#include <vulkan/vulkan_core.h>
#include <string>
#include <vector>
class VteDevice
{

public:
    VteDevice(std::string name);
    ~VteDevice();

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

    // Creating vulkan instance
    void CreateInstance();
    bool checkValidationLayerSupport();
    void SetupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
        VkDebugUtilsMessengerEXT debugMessenger, 
        const VkAllocationCallbacks* pAllocator);

    // Getting Physical Device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
};


