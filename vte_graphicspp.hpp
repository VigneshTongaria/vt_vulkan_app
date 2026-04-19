#pragma once

#include "vte_device.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class VteGraphicsPP {
public:
  VteGraphicsPP(VteDevice &device);
  ~VteGraphicsPP();
  VkPipeline graphicsPipeLine;
  VkRenderPass renderPass;
  std::vector<VkFramebuffer> swapChainFrameBuffers;

  void cleanPP();

private:
  VteDevice device;
  VkShaderModule fragShaderModule;
  VkShaderModule vertShaderModule;
  VkPipelineLayout pipeLineLayout;
  void createGraphicsPipeLine();
  void createRenderPass();
  void createFrameBuffers();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};
