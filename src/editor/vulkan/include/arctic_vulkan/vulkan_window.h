#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <utility>

class GLFWwindow;

class VulkanWindow
{
public:
    
    GLFWwindow* GetWindow();
    std::pair<uint32_t, const char**> GetGLFWExtensions() const;

    void CreateWindow();
    void CreateSurface(const VkInstance & vkInstance, VkSurfaceKHR & vkSurface);
    void CleanupWindow();

private:
    // glfw
    const uint32_t WINDOW_WIDTH = 1280;
    const uint32_t WINDOW_HEIGHT = 720;
    GLFWwindow* window = nullptr;
};