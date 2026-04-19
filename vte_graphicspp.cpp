#include "vte_graphicspp.hpp"
#include "vte_utlis.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

VteGraphicsPP::VteGraphicsPP(VteDevice &device) : device(device) {
  createRenderPass();
  createGraphicsPipeLine();
  createFrameBuffers();
}

VteGraphicsPP::~VteGraphicsPP() {}

void VteGraphicsPP::cleanPP() {
  vkDestroyShaderModule(device.getVkDevice(), fragShaderModule, nullptr);
  vkDestroyShaderModule(device.getVkDevice(), vertShaderModule, nullptr);
  vkDestroyPipelineLayout(device.getVkDevice(), pipeLineLayout, nullptr);
  vkDestroyRenderPass(device.getVkDevice(), renderPass, nullptr);
  vkDestroyPipeline(device.getVkDevice(), graphicsPipeLine, nullptr);

  for (auto fb : swapChainFrameBuffers) {
      vkDestroyFramebuffer(device.getVkDevice(),fb, nullptr);
  }
}

void VteGraphicsPP::createGraphicsPipeLine() {
  std::vector<char> vertShaderCode =
      vte::VteUtils::readFile("ShadersBinaries/vert.spv");
  std::vector<char> fragShaderCode =
      vte::VteUtils::readFile("ShadersBinaries/frag.spv");

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

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertPPCreateInfo,
                                                    fragPPCreateInfo};

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dsCreateInfo{};

  dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dsCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dsCreateInfo.pDynamicStates = dynamicStates.data();

  VkPipelineVertexInputStateCreateInfo viCreateInfo{};

  viCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  viCreateInfo.vertexBindingDescriptionCount = 0;
  viCreateInfo.pVertexAttributeDescriptions = nullptr;
  viCreateInfo.vertexAttributeDescriptionCount = 0;
  viCreateInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo iaCreateInfo{};
  iaCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  iaCreateInfo.primitiveRestartEnable = VK_FALSE;
  iaCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkViewport viewport{};

  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)device.swapChainExtent.width;
  viewport.height = (float)device.swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = device.swapChainExtent;

  VkPipelineViewportStateCreateInfo pvCreateStateInfo{};

  pvCreateStateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  pvCreateStateInfo.viewportCount = 1;
  pvCreateStateInfo.scissorCount = 1;
  pvCreateStateInfo.pViewports = &viewport;
  pvCreateStateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo prCreateInfo{};

  prCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  prCreateInfo.depthClampEnable = VK_FALSE;
  prCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  prCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  prCreateInfo.lineWidth = 1.0f;
  prCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  prCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  prCreateInfo.depthBiasEnable = VK_FALSE;
  prCreateInfo.depthBiasConstantFactor = 0.0f;
  prCreateInfo.depthBiasClamp = 0.0f;
  prCreateInfo.depthBiasSlopeFactor = 0.0f;

  // MSSA
  VkPipelineMultisampleStateCreateInfo pmsCreateInfo{};
  pmsCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  pmsCreateInfo.sampleShadingEnable = VK_FALSE;
  pmsCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  pmsCreateInfo.minSampleShading = 1.0f;
  pmsCreateInfo.pSampleMask = nullptr;
  pmsCreateInfo.alphaToCoverageEnable = VK_FALSE;
  pmsCreateInfo.alphaToOneEnable = VK_FALSE;

  // Depth and stencil testing
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo cbCreateInfo{};
  cbCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  cbCreateInfo.logicOpEnable = VK_FALSE;
  cbCreateInfo.logicOp = VK_LOGIC_OP_COPY;
  cbCreateInfo.attachmentCount = 1;
  cbCreateInfo.pAttachments = &colorBlendAttachment;
  cbCreateInfo.blendConstants[0] = 0.0f; // Optional
  cbCreateInfo.blendConstants[1] = 0.0f; // Optional
  cbCreateInfo.blendConstants[2] = 0.0f; // Optional
  cbCreateInfo.blendConstants[3] = 0.0f; // Optional

  VkPipelineLayoutCreateInfo pipeLineLayoutInfo{};
  pipeLineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeLineLayoutInfo.setLayoutCount = 0;
  pipeLineLayoutInfo.pSetLayouts = nullptr;
  pipeLineLayoutInfo.pushConstantRangeCount = 0;
  pipeLineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(device.getVkDevice(), &pipeLineLayoutInfo, nullptr,
                             &pipeLineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &viCreateInfo;
  pipelineInfo.pInputAssemblyState = &iaCreateInfo;
  pipelineInfo.pViewportState = &pvCreateStateInfo;
  pipelineInfo.pRasterizationState = &prCreateInfo;
  pipelineInfo.pMultisampleState = &pmsCreateInfo;
  pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pColorBlendState = &cbCreateInfo;
  pipelineInfo.pDynamicState = &dsCreateInfo;
  pipelineInfo.layout = pipeLineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(device.getVkDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &graphicsPipeLine) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void VteGraphicsPP::createRenderPass() {
  VkAttachmentDescription colordesc{};
  colordesc.format = device.swapChainImageFormat;
  colordesc.samples = VK_SAMPLE_COUNT_1_BIT;
  colordesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colordesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colordesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colordesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colordesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colordesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo rpCreateInfo{};
  rpCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rpCreateInfo.attachmentCount = 1;
  rpCreateInfo.pAttachments = &colordesc;
  rpCreateInfo.subpassCount = 1;
  rpCreateInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(device.getVkDevice(), &rpCreateInfo, nullptr,
                         &renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void VteGraphicsPP::createFrameBuffers() {
  swapChainFrameBuffers.resize(device.swapChainImageViews.size());

  for (int i = 0; i < device.swapChainImageViews.size(); i++) {
    VkFramebufferCreateInfo frameBufferInfo{};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.attachmentCount = 1;
    frameBufferInfo.renderPass = renderPass;
    frameBufferInfo.pAttachments = &device.swapChainImageViews[i];
    frameBufferInfo.width = device.swapChainExtent.width;
    frameBufferInfo.height = device.swapChainExtent.height;
    frameBufferInfo.layers = 1;

    if (vkCreateFramebuffer(device.getVkDevice(), &frameBufferInfo, nullptr,
                            &swapChainFrameBuffers[i])) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

VkShaderModule
VteGraphicsPP::createShaderModule(const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;

  if (vkCreateShaderModule(device.getVkDevice(), &createInfo, nullptr,
                           &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}
