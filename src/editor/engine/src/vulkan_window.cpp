#include "vulkan_window.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <xcb/xcb.h>
#include <X11/Xlib-xcb.h>

GLFWwindow * VulkanWindow::GetWindow()
{
    return this->window;
}

std::pair<uint32_t, const char**> VulkanWindow::GetGLFWExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::make_pair(glfwExtensionCount, glfwExtensions);
}

void VulkanWindow::CreateWindow()
{
    // init glfw
    glfwInit();

    // set hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
}

void VulkanWindow::CreateSurface(const VkInstance & vkInstance, VkSurfaceKHR & vkSurface)
{
        // create native surface
    Display* x11Display = glfwGetX11Display();
    if (!x11Display) {
        std::cout << "error: vulkan: failed to create window 32 surface!";
        return;
    }
    xcb_connection_t* xcbConnection = XGetXCBConnection(x11Display);
    xcb_window_t xcbWindow = glfwGetX11Window(window);

    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.connection = xcbConnection;
    createInfo.window = xcbWindow;

    VkResult resultWindows = vkCreateXcbSurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface);

    if(resultWindows != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create surface!";
        return;
    }

    // create glfw surface from native surface
    VkResult resultGlfw = glfwCreateWindowSurface(vkInstance, window, nullptr, &vkSurface);
    if(resultGlfw != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create glfw window surface!";
        return;
    }

    // set instance
    this->vkInstance = vkInstance;
}

void VulkanWindow::CleanupWindow()
{
    // glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}
