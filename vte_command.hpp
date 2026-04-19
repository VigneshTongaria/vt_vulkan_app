#pragma once

#include "vte_device.hpp"
#include "vte_graphicspp.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

class VteCommand {
public:
  VteCommand(VteDevice &device, VteGraphicsPP& graphicsPP);
  ~VteCommand();
  void cleanCommandPools();
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;

private:
  void createCommandPool();
  void createCommandBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer,uint32_t imageIndex);
  VteDevice device;
  VteGraphicsPP graphicsPP;
};
