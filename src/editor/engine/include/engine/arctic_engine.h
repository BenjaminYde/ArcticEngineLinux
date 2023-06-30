#pragma once

class VulkanWindow;
class VulkanContext;

class ArcticEngine
{
public:
    void Initialize();
    void Run();
    void Cleanup();

private:
    VulkanWindow* pVulkanWindow;
    VulkanContext* pVulkanContext;
};