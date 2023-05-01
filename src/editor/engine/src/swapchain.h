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
    VkFormat imageFormat;
    VkExtent2D extent;
};

class SwapChainValidator
{
public:
    static SwapChainDeviceSupport QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR & vkSurface);
};


class GLFWwindow;

class SwapChain
{
public:
    // todo: move constructor members to "Load() function"
    // todo: move constructor members to "Load() function"
    // todo: move constructor members to "Load() function"
    SwapChain(
        const VkDevice &vkDevice, 
        const VkPhysicalDevice &vkPhysicalDevice, 
        const VkSurfaceKHR & vkSurface,
        GLFWwindow* window);

    void Load();
    void CleanUp();

    const SwapChainData GetData();
    const VkSwapchainKHR &GetSwapChain();
    const std::vector<VkImageView> &GetImageViews();

private:
    void createSwapChain();
    void createImageViews();

    VkSurfaceFormatKHR selectSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR selectSwapChainPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D selectSwapChainExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    // todo: remove members
    // todo: remove members
    // todo: remove members
    VkDevice vkDevice;
    VkPhysicalDevice vkPhysicalDevice;
    VkSurfaceKHR vkSurface;
    GLFWwindow* window = nullptr;

    SwapChainData swapChainData;

    VkSwapchainKHR vkSwapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
};