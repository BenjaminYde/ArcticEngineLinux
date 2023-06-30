#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapChainDeviceSupport
{
    VkSurfaceCapabilitiesKHR capabilities;          // image width/height, min/max images, ...
    std::vector<VkSurfaceFormatKHR> surfaceFormats; // pixel format, color space, ...
    std::vector<VkPresentModeKHR> presentModes;     // FIFO, Mailbox, ...
};

struct SwapChainData
{
    uint32_t imageCount;
    VkFormat imageFormat;
    VkExtent2D extent;
};

class VulkanWindow;

class VulkanSwapChain
{
public:

    void Configure(
        const VkDevice& vkDevice, 
        const VkPhysicalDevice& vkPhysicalDevice,
        const VkSurfaceKHR& vkSurface,
        VulkanWindow* window);

    void CreateSwapChain();

    SwapChainDeviceSupport QuerySwapChainSupport(const VkPhysicalDevice & device, const VkSurfaceKHR & vkSurface) const;

    void CleanUp(const VkDevice &vkDevice);

    const SwapChainData GetData();
    const VkSwapchainKHR &GetSwapChain();
    const std::vector<VkImageView> &GetImageViews();

private:

    VkDevice vkDevice;
    VkPhysicalDevice vkPhysicalDevice;
    VkSurfaceKHR vkSurface;
    VulkanWindow* window;

    void createSwapChain(
        const VkDevice & vkDevice, 
        const VkPhysicalDevice & vkPhysicalDevice, 
        const VkSurfaceKHR & vkSurface,
        VulkanWindow* window);

    void createImageViews(const VkDevice &vkDevice);

    VkSurfaceFormatKHR selectSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR selectSwapChainPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D selectSwapChainExtent(VulkanWindow* window, const VkSurfaceCapabilitiesKHR &capabilities);

    SwapChainData swapChainData;

    VkSwapchainKHR vkSwapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
};