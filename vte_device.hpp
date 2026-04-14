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
    void CreateInstance();
    bool checkValidationLayerSupport();
};


