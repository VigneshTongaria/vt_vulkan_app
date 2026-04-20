#include "vte_command.hpp"
#include "vte_graphicspp.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VteCommand::VteCommand(VteDevice &device, VteGraphicsPP &graphicsPP)
    : device(device), graphicsPP(graphicsPP) {
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();

  vte::Vtewindow::resizeCallbacks.push_back(OnFrameBufferResizeCallback);
}

VteCommand::~VteCommand() {}

bool VteCommand::frameBufferResized = false;

void VteCommand::cleanCommandPools() {
  vkDestroyCommandPool(device.getVkDevice(), commandPool, nullptr);
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i ++) {
      vkDestroySemaphore(device.getVkDevice(), imageAvailableSemaphores[i], nullptr);
      vkDestroySemaphore(device.getVkDevice(), renderFinishedSemaphores[i], nullptr);
      vkDestroyFence(device.getVkDevice(), inFlightFences[i], nullptr);

  }
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

void VteCommand::drawFrame() {
  vkWaitForFences(device.getVkDevice(), 1, &inFlightFences[currentFrame], VK_FALSE,
                  UINT64_MAX);

  uint32_t imageIndex;
  VkResult acquireImageResult =  vkAcquireNextImageKHR(device.getVkDevice(), device.swapChain, UINT64_MAX,
                        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

  if(acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireImageResult == VK_SUBOPTIMAL_KHR
      || frameBufferResized)
  {
      frameBufferResized = false;
      resetPresentInfo();
      return;
  }
  else if (acquireImageResult != VK_SUCCESS) {
      throw std::runtime_error("failed to acquire swap chain images");

  }

  vkResetFences(device.getVkDevice(), 1, &inFlightFences[currentFrame]);

  vkResetCommandBuffer(commandBuffers[currentFrame], 0);

  recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

  VkSubmitInfo submitInfo{};
  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {device.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  VkResult presentResult = vkQueuePresentKHR(device.presentQueue, &presentInfo);

  if(presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || frameBufferResized)
  {
      frameBufferResized = false;
      resetPresentInfo();
      return;
  }
  else if (presentResult != VK_SUCCESS) {
      throw std::runtime_error("failed to acquire swap chain images");

  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VteCommand::createCommandBuffers() {

  commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo bAllocInfo{};
  bAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bAllocInfo.commandPool = commandPool;
  bAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bAllocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(device.getVkDevice(), &bAllocInfo,
                               &commandBuffers[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers !");
  }
}

void VteCommand::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                     uint32_t imageIndex) {
  VkCommandBufferBeginInfo bBeginInfo{};
  bBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  bBeginInfo.flags = 0;
  bBeginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &bBeginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

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

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void VteCommand::resetPresentInfo()
{
    vkDeviceWaitIdle(device.getVkDevice());
    graphicsPP.destroyFrameBuffers();
    device.recreateSwapChains();
    graphicsPP.recreateFrameBuffers();
}

void VteCommand::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device.getVkDevice(), &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) ||
        vkCreateSemaphore(device.getVkDevice(), &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) ||
        vkCreateFence(device.getVkDevice(), &fenceInfo, nullptr,
                      &inFlightFences[i]) != VK_SUCCESS) {
      std::runtime_error("failed to create sync objects");
    }
  }
}

void VteCommand::OnFrameBufferResizeCallback(int width, int height)
{
    frameBufferResized = true;
}
