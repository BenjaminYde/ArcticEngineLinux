#pragma once

#include <vulkan/vulkan_core.h>

class VulkanMemoryHandler
{
public:
    VulkanMemoryHandler(VkDevice& vkDevice, VkPhysicalDevice& vkPhysicalDevice, VkQueue& vkGraphicsQueue, VkQueue& vkTransferQueue);

    bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool& commandPool);
    bool FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& memoryTypeIndex);

private:
    VkDevice vkDevice;
    VkPhysicalDevice vkPhysicalDevice;
    VkQueue vkGraphicsQueue;
    VkQueue vkTransferQueue;
};