#pragma once

class VulkanLoader;
class VulkanWindow;

class ArcticEngine
{
public:
    void Initialize();
    void Run();
    void Cleanup();

private:
    VulkanWindow* pVulkanWindow;
    VulkanLoader* vulkanLoader;
};