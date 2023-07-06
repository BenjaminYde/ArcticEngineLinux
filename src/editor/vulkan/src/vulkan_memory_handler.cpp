#include "arctic_vulkan/vulkan_memory_handler.h"

VulkanMemoryHandler::VulkanMemoryHandler(VkPhysicalDevice& vkPhysicalDevice)
:
vkPhysicalDevice(vkPhysicalDevice)
{
    //this->memoryType = findMemoryType();
}

/// @brief 
/// @param typeFilter used to specify the bit field of memory types that are suitable
/// @param properties 
/// @param memoryType 
/// @return 
bool VulkanMemoryHandler::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, uint32_t& memoryTypeIndex)
{
    // get memory properties of physical device
    // >> has memory types: distinct memory resources like dedicated VRAM
    // >> has memory heaps: the different types of memory exist within these heaps
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);

    // loop over all properties
    // >> try find matching memorty index
    for (uint32_t i=0; i < memProperties.memoryTypeCount; i++) 
    {
        // typeFilter: we can find the index of a suitable memory by checking if the corresponding bit is set to 1
        // 
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
        {
            memoryTypeIndex = i;
            return true;
        }
    }
    return false;
}