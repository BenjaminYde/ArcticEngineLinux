#pragma once

//#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <utility>
#include<vector>

class SDL_Window;

class VulkanWindow
{
public:
    
    SDL_Window* GetWindow();
    std::vector<const char*> GetExtensions() const;

    void CreateWindow();
    void CreateSurface(const VkInstance & vkInstance, VkSurfaceKHR & vkSurface);
    void CleanupWindow();

private:
    
    const uint32_t WINDOW_WIDTH = 1280;
    const uint32_t WINDOW_HEIGHT = 720;
    SDL_Window* window = nullptr;
};