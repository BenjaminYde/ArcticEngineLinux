#include "arctic_vulkan/vulkan_context.h"

#include "arctic_vulkan/vulkan_loader.h"
#include "arctic_vulkan/vulkan_renderloop.h"
#include "arctic_vulkan/vulkan_window.h"

VulkanContext::VulkanContext(VulkanWindow *vulkanWindow)
{
    pVulkanLoader = new VulkanLoader(vulkanWindow);
}

void VulkanContext::Cleanup()
{
    // cleanup vulkan
    pVulkanLoader->Cleanup();
    delete pVulkanLoader;
}

void VulkanContext::Render()
{
    // get renderloop
    VulkanRenderLoop* renderLoop = pVulkanLoader->GetRenderLoop();

    // reload swapchain when dirty
    if(renderLoop->IsSwapChainDirty())
    {
        pVulkanLoader->ReloadSwapChain();
    }

    // render
    renderLoop->Render();
}