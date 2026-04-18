#include "vte_device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class VteGraphicsPP {
public:
  VteGraphicsPP(VteDevice &device);
  ~VteGraphicsPP();

  void cleanPP();

private:
  VteDevice device;
  VkShaderModule fragShaderModule;
  VkShaderModule vertShaderModule;
  void createGraphicsPipeLine();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};
