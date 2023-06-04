#pragma once

#include <vulkan/vulkan_core.h>

class SwapChain;
class RenderPipeline;

class RenderLoop
{
public:
    RenderLoop(
        VkDevice vkDevice,
        SwapChain* swapChain, 
        RenderPipeline* renderPipeline,
        VkQueue GraphicsQueue,
        VkQueue vkPresentQueue);

    void Render();
    void CleanUp();

private:

    // devices
    VkDevice vkDevice = VK_NULL_HANDLE;
    SwapChain* pSwapchain;
    RenderPipeline* pRenderPipeline;

    // commands
    VkCommandPool vkCommandPool;
    VkCommandBuffer vkCommandBuffer;

    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;

    // syncing
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence isDoneRenderingFence;

    // commands
    void vulkanCreateCommandPool(uint32_t graphicsFamilyIndex);
    void vulkanCreateCommandBuffer();

    void vulkanRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    
    // syncing
    void vulkanCreateSyncObjects();
};