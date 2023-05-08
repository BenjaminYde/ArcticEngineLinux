#pragma once

#include <vector>
#include <optional>
#include <set>
#include <vulkan/vulkan_core.h>

class VulkanWindow;
class RenderPipeline;
class SwapChain;
class RenderLoop;

class VulkanLoader
{
public:
    void Load(VulkanWindow & vulkanWindow, RenderLoop* &renderLoop);
    void Cleanup();

private:

    // vulkan
    VkInstance vkInstance = nullptr;

    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
    VkDevice vkDevice = VK_NULL_HANDLE;

    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;

    VkCommandPool vkCommandPool;
    VkCommandBuffer vkCommandBuffer;

    const std::vector<const char*> requiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete()
        {
            return  graphicsFamily.has_value() &&
                    presentFamily.has_value();
        }
    };

    VkSurfaceKHR vkSurface;
    SwapChain* pSwapchain;
    RenderPipeline* pRenderPipeline;

    void vulkanCreateInstance(VulkanWindow & vulkanWindow);
    void vulkanLoadDebugMessenger();
    void vulkanLoadPhysicalDevice();
    void vulkanCreateLogicalDevice(const VkPhysicalDevice & vkPhysicalDevice, QueueFamilyIndices indices);

    void vulkanCreateCommandPool(QueueFamilyIndices indices);
    void vulkanCreateCommandBuffer();

    // devices
    std::vector<const char*> vulkanGetRequiredExtensions(const VulkanWindow & vulkanWindow);
    bool isVkDeviceSuitable(const VkPhysicalDevice& device,
                            const VkSurfaceKHR & vkSurface,
                            VkPhysicalDeviceProperties deviceProperties,
                            VkPhysicalDeviceFeatures deviceFeatures,
                            QueueFamilyIndices queueFamilyIndices) const;
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR & surface);
    bool findRequiredDeviceExtensions(const VkPhysicalDevice& device) const;

    // validation layers
    const bool enableValidationLayers = false;

    VkDebugUtilsMessengerEXT debugMessenger;
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation" // all the useful standard validation is bundled into a layer included in the SDK
    };

    void vulkanDestroyDebugMessenger();
    bool vulkanFoundValidationLayers();
    VkResult vulkanCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                const VkAllocationCallbacks* pAllocator,
                                                VkDebugUtilsMessengerEXT* pDebugMessenger);
    void vulkanPopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void vulkanDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                             VkDebugUtilsMessengerEXT debugMessenger,
                                             const VkAllocationCallbacks* pAllocator);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
};