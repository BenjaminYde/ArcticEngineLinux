#pragma once

#include <vulkan/vulkan_core.h>
#include "vk_mem_alloc.h"

class VulkanMemoryHandler
{
public:
    VulkanMemoryHandler(VkDevice& vkDevice, VkPhysicalDevice& vkPhysicalDevice, VkInstance& vkInstance, VkQueue& vkGraphicsQueue, VkQueue& vkTransferQueue);
    void Cleanup();
    
    bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool);
    bool FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& memoryTypeIndex);

    VmaAllocator vmaAllocator;
private:
    VkDevice vkDevice;
    VkPhysicalDevice vkPhysicalDevice;
    VkQueue vkGraphicsQueue;
    VkQueue vkTransferQueue;
};