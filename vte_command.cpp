#include "vte_command.hpp"
#include "vte_graphicspp.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VteCommand::VteCommand(VteDevice &device, VteGraphicsPP &graphicsPP)
    : device(device), graphicsPP(graphicsPP) {
  createCommandPool();
  createCommandBuffer();
}

VteCommand::~VteCommand() {}

void VteCommand::cleanCommandPools() {
  vkDestroyCommandPool(device.getVkDevice(), commandPool, nullptr);
}

void VteCommand::createCommandPool() {
  VkCommandPoolCreateInfo cPoolInfo{};
  cPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  cPoolInfo.queueFamilyIndex = device.queueFamilies.graphicsFamily.value();

  if (vkCreateCommandPool(device.getVkDevice(), &cPoolInfo, nullptr,
                          &commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VteCommand::createCommandBuffer() {
  VkCommandBufferAllocateInfo bAllocInfo{};
  bAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bAllocInfo.commandPool = commandPool;
  bAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bAllocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device.getVkDevice(), &bAllocInfo,
                               &commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers !");
  }
}

void VteCommand::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                     uint32_t imageIndex) {
  VkCommandBufferBeginInfo bBeginInfo{};
  bBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  bBeginInfo.flags = 0;
  bBeginInfo.pInheritanceInfo = nullptr;

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = graphicsPP.renderPass;
  renderPassInfo.framebuffer = graphicsPP.swapChainFrameBuffers[imageIndex];
  renderPassInfo.renderArea.extent = device.swapChainExtent;
  renderPassInfo.renderArea.offset = {0, 0};

  VkClearValue clearColor = {0.0, 0.5, 0.5, 1.0f};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPP.graphicsPipeLine);
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(device.swapChainExtent.width);
  viewport.height = static_cast<float>(device.swapChainExtent.height);
  viewport.minDepth = 0.0f;

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = device.swapChainExtent;

  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}
