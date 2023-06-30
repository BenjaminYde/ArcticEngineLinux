#pragma once

class VulkanWindow;
class VulkanLoader;
class VulkanRenderLoop;

class VulkanContext
{
public: 

    VulkanContext(VulkanWindow* vulkanWindow);
    void Cleanup();
    void Render();

private:
    VulkanLoader* pVulkanLoader;
};