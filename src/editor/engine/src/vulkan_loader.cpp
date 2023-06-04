#include "vulkan_loader.h"
#include "utilities/file_utility.h"
#include "utilities/application.h"

#include <iostream>
#include <fmt/core.h>

#include "vulkan_window.h"
#include "renderpipeline.h"
#include "renderloop.h"
#include "swapchain.h"

const RenderLoop& VulkanLoader::GetRenderLoop()
{
    return *pRenderLoop;
}

VulkanLoader::VulkanLoader(VulkanWindow & vulkanWindow)
{
    // check validation layers
    if(enableValidationLayers && !vulkanFoundValidationLayers())
    {
        std::cout << "error: vulkan: validation layers requested, but not available!";
        return;
    }

    vulkanCreateInstance(vulkanWindow);
    vulkanLoadDebugMessenger();

    vulkanWindow.CreateSurface(vkInstance, vkSurface);

    pSwapchain = new SwapChain();

    vulkanLoadPhysicalDevice(vkInstance, vkSurface, *pSwapchain);
    
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vkPhysicalDevice, vkSurface);
    vulkanCreateLogicalDevice(vkPhysicalDevice, queueFamilyIndices);
    
    // create swapchain
    pSwapchain->Load(
        vkDevice,
        vkPhysicalDevice,
        vkSurface,
        vulkanWindow.GetWindow());

    // create render pipeline
    pRenderPipeline = new RenderPipeline(
        vkDevice,
        queueFamilyIndices.graphicsFamily.value(),
        pSwapchain->GetData(), 
        pSwapchain->GetImageViews());
    pRenderPipeline->Load();
    
    // create render loop
    pRenderLoop = new RenderLoop(
        vkDevice, 
        pSwapchain, 
        pRenderPipeline, 
        vkGraphicsQueue, 
        vkPresentQueue);
}

void VulkanLoader::Cleanup()
{
    // wait until device is not executing work
    vkDeviceWaitIdle(vkDevice);

    // render loop
    pRenderLoop->CleanUp();
    delete pRenderLoop;

    // render pipeline
    pRenderPipeline->CleanUp();
    delete pRenderPipeline;

    // images & swapchain
    pSwapchain->CleanUp(vkDevice);
    delete pSwapchain;

    // devices
    vkDestroyDevice(vkDevice, nullptr);

    // debug
    vulkanDestroyDebugMessenger();

    // surface
    vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);

    //instance
    vkDestroyInstance(vkInstance, nullptr);
}

void VulkanLoader::vulkanCreateInstance(VulkanWindow & vulkanWindow)
{
    // create app info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ArcticGame";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "ArcticEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // create vk instance create info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.pNext = nullptr;

    // apply validation layers
    if(enableValidationLayers)
    {
        // set layers
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // set debug messenger info
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        vulkanPopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    // set extensions
    auto extensions = vulkanGetRequiredExtensions(vulkanWindow);
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // create vk instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    if( result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create instance!";
        return;
    }
}

std::vector<const char*> VulkanLoader::vulkanGetRequiredExtensions(const VulkanWindow & vulkanWindow)
{
    // create empty extensions
    std::vector<const char*> extensions;

    // get glfw extensions
    auto glfwExtensionsData = vulkanWindow.GetGLFWExtensions();
    uint32_t glfwExtensionCount = glfwExtensionsData.first;
    const char** glfwExtensions = glfwExtensionsData.second;

    for(int i=0; i<glfwExtensionCount; ++i)
    {
        const char* extension = glfwExtensions[i];
        extensions.push_back(extension);
    }

    // get validation extension
    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // debug extensions
    bool debugExtentions = false;
    if(debugExtentions)
    {
        std::cout << "info: vulkan: available extensions: " << std::endl;
        for( const auto& extension : extensions)
        {
            std::cout << "\t" << extension << std::endl;
        }
    }

    return extensions;
}

#pragma region vulkan_devices

void VulkanLoader::vulkanLoadPhysicalDevice(
    const VkInstance& instance,
    const VkSurfaceKHR& surface,
    const SwapChain& swapChain)
{
    // get available physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        std::cout << "error: vulkan: did not find physical device!";
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // find suitable device
    // todo: idea: could add score implementation (more features = better score), select device with highest score
    vkPhysicalDevice = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    QueueFamilyIndices queueFamilyIndices;

    for(auto & device : devices)
    {
        // get data of device
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        queueFamilyIndices = findQueueFamilies(device, surface);

        // break loop when found suitable device
        if(isVkDeviceSuitable(device, surface, swapChain ,deviceProperties, deviceFeatures, queueFamilyIndices))
        {
            vkPhysicalDevice = device;
            break;
        }
    }

    // final check if device is valid
    if(vkPhysicalDevice == VK_NULL_HANDLE)
    {
        std::cout << "error: vulkan: did not find suitable physical device!";
        return;
    }
}

void VulkanLoader::vulkanCreateLogicalDevice(const VkPhysicalDevice & vkPhysicalDevice, QueueFamilyIndices indices)
{
    // create device queue infos
    // >> create set of queue families (re-use queue families instead of creating duplicates)
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies)
    {
        // create device queue create info
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // add create info
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // create device features
    // >> currently empty
    VkPhysicalDeviceFeatures deviceFeatures{};

    // create device info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    // create device
    VkResult result = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice);
    if(result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create logical device!";
        return;
    }

    // get graphics queue
    vkGetDeviceQueue(vkDevice, indices.graphicsFamily.value(), 0, &vkGraphicsQueue);

    // get present queue
    vkGetDeviceQueue(vkDevice, indices.presentFamily.value(), 0, &vkPresentQueue);
}

bool VulkanLoader::isVkDeviceSuitable(
        const VkPhysicalDevice & device,
        const VkSurfaceKHR & surface,
        const SwapChain & swapChain,
        VkPhysicalDeviceProperties deviceProperties,
        VkPhysicalDeviceFeatures deviceFeatures,
        QueueFamilyIndices queueFamilyIndices) const
{
    // check device properties
    if(deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        return false;

    // check device features
    if(!deviceFeatures.geometryShader)
        return false;

    // check if queue families are complete
    if(!queueFamilyIndices.IsComplete())
        return false;

    // try find device extensions
    bool foundDeviceExtensions = findRequiredDeviceExtensions(device);
    if(!foundDeviceExtensions)
        return false;

    // check if swap chain is valid
    // >> see device & surface
    SwapChainDeviceSupport swapChainSupport = swapChain.QuerySwapChainSupport(device, surface);
    bool isSwapChainValid = !swapChainSupport.surfaceFormats.empty() && !swapChainSupport.presentModes.empty();
    if(!isSwapChainValid)
        return false;

    return true;
}

VulkanLoader::QueueFamilyIndices VulkanLoader::findQueueFamilies(const VkPhysicalDevice & device, const VkSurfaceKHR & surface)
{
    // get queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // find suitable families
    QueueFamilyIndices queueFamilyIndices{};
    uint32_t familyIndex = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        // set graphics family
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            queueFamilyIndices.graphicsFamily = familyIndex;

        // set present family
        VkBool32 isPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, familyIndex, surface, &isPresentSupport);
        if(isPresentSupport)
            queueFamilyIndices.presentFamily = familyIndex;

        ++familyIndex;
    }
    return queueFamilyIndices;
}

bool VulkanLoader::findRequiredDeviceExtensions(const VkPhysicalDevice & device) const
{
    // get available device extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0)
        return false;

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // check if available extensions met requirements
    std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
    for(const auto & extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

#pragma endregion vulkan_devices

#pragma region vulkan_pipeline


#pragma endregion vulkan_pipeline

#pragma region vulkan_validation

void VulkanLoader::vulkanLoadDebugMessenger()
{
    // return when no validation layers
    if (!enableValidationLayers)
        return;

    // create debug messenger info
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    vulkanPopulateDebugMessengerCreateInfo(debugCreateInfo);

    // create debug messenger
    auto result = vulkanCreateDebugUtilsMessengerEXT(vkInstance, &debugCreateInfo, nullptr, &debugMessenger);
    if( result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create debug messenger!";
        return;
    }
}

void VulkanLoader::vulkanDestroyDebugMessenger()
{
    if (enableValidationLayers)
        vulkanDestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
}

bool VulkanLoader::vulkanFoundValidationLayers()
{
    // get available layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check if all layers are present
    // >> return true
    for(const auto validationLayer : validationLayers)
    {
        bool layerFound = false;
        for(const auto & availableLayer : availableLayers)
        {
            if(strcmp(validationLayer, availableLayer.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }
    return true;
}

VkResult VulkanLoader::vulkanCreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanLoader::vulkanDestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);
}

void VulkanLoader::vulkanPopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLoader::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

#pragma endregion vulkan_validation