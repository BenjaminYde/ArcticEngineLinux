#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "swapchain_data.h"

class RenderPipeline
{
public:
  RenderPipeline(
    const VkDevice & device, 
    const SwapChainData & swapChainData, 
    const std::vector<VkImageView> & swapChainImageViews); 

  void Load();
  void CleanUp();

  const VkRenderPass & GetRenderPass();
  const VkPipeline & GetPipeline();
  const VkFramebuffer & GetFrameBuffer(uint32_t index);

private:
    VkDevice vkDevice = VK_NULL_HANDLE;
    const SwapChainData swapChainData;
    const std::vector<VkImageView> swapChainImageViews;

    VkRenderPass vkRenderPass;
    VkPipelineLayout vkPipelineLayout;
    VkPipeline vkPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    void createRenderPass();
    void createPipeline();
    void createFramebuffers();

    bool createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);
};