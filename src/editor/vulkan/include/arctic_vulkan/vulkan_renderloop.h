#pragma once

#include <vulkan/vulkan_core.h>

class VulkanSwapChain;
class VulkanRenderPipeline;

class VulkanRenderLoop
{
public:
    VulkanRenderLoop(
        VkDevice vkDevice,
        VulkanSwapChain* swapChain, 
        VulkanRenderPipeline* renderPipeline,
        VkQueue GraphicsQueue,
        VkQueue vkPresentQueue);
    
    void Render();
    void CleanUp();

    bool IsSwapChainDirty() const;

private:

    // devices
    VkDevice vkDevice = VK_NULL_HANDLE;
    VulkanSwapChain* pSwapchain;
    VulkanRenderPipeline* pRenderPipeline;

    // commands
    VkCommandPool vkCommandPool;
    VkCommandBuffer vkCommandBuffer;

    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;

    // syncing
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence isDoneRenderingFence;

    bool isSwapChainDirty;

    // commands
    void vulkanCreateCommandPool(uint32_t graphicsFamilyIndex);
    void vulkanCreateCommandBuffer();

    void vulkanRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    
    // syncing
    void vulkanCreateSyncObjects();
};