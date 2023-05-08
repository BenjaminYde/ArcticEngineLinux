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
        VkCommandPool vkCommandPool,
        VkCommandBuffer vkCommandBuffer,
        VkQueue GraphicsQueue,
        VkQueue vkPresentQueue);

    void Draw();
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

    void vulkanRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void vulkanCreateSyncObjects();
};