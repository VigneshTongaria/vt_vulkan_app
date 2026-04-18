#include "vte_graphicspp.hpp"
#include "vte_utlis.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

VteGraphicsPP::VteGraphicsPP(VteDevice &device) : device(device)
{
    createGraphicsPipeLine();
}

VteGraphicsPP::~VteGraphicsPP()
{

}

void VteGraphicsPP::cleanPP()
{
    vkDestroyShaderModule(device.getVkDevice(),fragShaderModule,nullptr);
    vkDestroyShaderModule(device.getVkDevice(),vertShaderModule,nullptr);
}

void VteGraphicsPP::createGraphicsPipeLine() {
  std::vector<char> vertShaderCode =
      vte::VteUtils::readFile("shaders/vert.spv");
  std::vector<char> fragShaderCode =
      vte::VteUtils::readFile("shaders/frag.spv");

  vertShaderModule = createShaderModule(vertShaderCode);
  fragShaderModule = createShaderModule(fragShaderCode);

  VkPipelineShaderStageCreateInfo vertPPCreateInfo{};
  vertPPCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertPPCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertPPCreateInfo.module = vertShaderModule;
  vertPPCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragPPCreateInfo{};
  fragPPCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragPPCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragPPCreateInfo.module = fragShaderModule;
  fragPPCreateInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages [] = {vertPPCreateInfo,fragPPCreateInfo};

}

VkShaderModule VteGraphicsPP::createShaderModule(const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;

  if (vkCreateShaderModule(device.getVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}
