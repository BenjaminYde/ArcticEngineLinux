#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

class VulkanSwapChain;
class VulkanRenderPipeline;
class VulkanMemoryHandler;
class Vertex;

class VulkanRenderLoop
{
public:
    VulkanRenderLoop(
        VkDevice vkDevice,
        VulkanSwapChain* swapChain, 
        VulkanRenderPipeline* renderPipeline,
        VulkanMemoryHandler* vkMemoryHandler, 
        VkQueue GraphicsQueue,
        VkQueue vkTransferQueue,
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
    VkCommandPool vkCommandPoolGraphics;
    VkCommandPool vkCommandPoolTransfer;
    std::vector<VkCommandBuffer> vkCommandBuffers;

    VkQueue vkGraphicsQueue;
    VkQueue vkTransferQueue;
    VkQueue vkPresentQueue;

    // syncing
    const int MAX_FRAMES_IN_FLIGHT = 3;

    std::vector<VkSemaphore> imageAvailableSemaphore;
    std::vector<VkSemaphore> renderFinishedSemaphore;
    std::vector<VkFence> isDoneRenderingFence;

    uint16_t currentFrameIndex = 0;
    bool isSwapChainDirty;

    // memory
    VulkanMemoryHandler* vkMemoryHandler;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkDescriptorPool vkDescriptorPool;
    std::vector<VkDescriptorSet> vkDescriptorSets;

    // commands
    void createCommandPool(uint32_t graphicsFamilyIndex, uint32_t transferFamilyIndex);
    void createCommandBuffers();
    
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t frameIndex);
    void updateUniformBuffer(uint32_t frameIndex);

    // memory
    bool createVertexBuffer(std::vector<Vertex> vertices);
    bool createIndexBuffer(std::vector<uint32_t> indices);

    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();

    // syncing
    void createSyncObjects();
};