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
    //std::vector<VkCommandBuffer> vkCommandBuffers;

    VkQueue vkGraphicsQueue;
    VkQueue vkTransferQueue;
    VkQueue vkPresentQueue;

    // syncing
    const int MAX_FRAMES_IN_FLIGHT = 3;

    uint16_t currentFrameIndex = 0;

    struct Frame
    {   
        // commands
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        // syncing
        VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence isDoneRenderingFence = VK_NULL_HANDLE;

        // memory
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        VkBuffer uniformBuffer = VK_NULL_HANDLE;
        VkDeviceMemory uniformBufferMemory = VK_NULL_HANDLE;
        void* uniformBufferMapped = nullptr;
    };

    std::vector<Frame*> frames;

    bool isSwapChainDirty;

    // memory
    VulkanMemoryHandler* vkMemoryHandler;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkDescriptorPool vkDescriptorPool;

    // commands
    void createCommandPool(uint32_t graphicsFamilyIndex, uint32_t transferFamilyIndex);
    void createCommandBuffers();
    
    void recordCommandBuffer(const Frame& frame, uint32_t imageIndex);
    void updateUniformBuffer(const Frame& frame);

    // memory
    bool createVertexBuffer(std::vector<Vertex> vertices);
    bool createIndexBuffer(std::vector<uint32_t> indices);

    bool createUniformBuffers();
    bool createDescriptorPool();
    bool createDescriptorSets();

    // syncing
    void createSyncObjects();
};