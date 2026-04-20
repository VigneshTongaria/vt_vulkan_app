#pragma once

#include "vte_device.hpp"
#include "vte_graphicspp.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

class VteCommand {
public:
  VteCommand(VteDevice &device, VteGraphicsPP &graphicsPP);
  ~VteCommand();
  void cleanCommandPools();
  void drawFrame();
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
  const int MAX_FRAMES_IN_FLIGHT = 2;
  static bool frameBufferResized;

private:
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void resetPresentInfo();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  VteDevice& device;
  VteGraphicsPP& graphicsPP;
  uint32_t currentFrame = 0;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  static void OnFrameBufferResizeCallback(int width, int height);
};
