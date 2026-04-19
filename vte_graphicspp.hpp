#pragma once

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
  VkPipelineLayout pipeLineLayout;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeLine;
  std::vector<VkFramebuffer> swapChainFrameBuffers;
  void createGraphicsPipeLine();
  void createRenderPass();
  void createFrameBuffers();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};
