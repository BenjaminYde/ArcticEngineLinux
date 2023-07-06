#pragma once

#include <vulkan/vulkan_core.h>

class VulkanMemoryHandler
{
public:
    VulkanMemoryHandler(VkPhysicalDevice& vkPhysicalDevice);

    bool FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& memoryTypeIndex);

private:
    VkPhysicalDevice vkPhysicalDevice;
};