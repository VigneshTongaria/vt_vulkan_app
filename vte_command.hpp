#pragma once

#include "vte_device.hpp"
#include <vulkan/vulkan_core.h>

class VteCommand {
public:
  VteCommand(VteDevice& device);
  ~VteCommand();
  VkCommandPool commandPool;

private:
  void createCommandPool();
  VteDevice device;
};
