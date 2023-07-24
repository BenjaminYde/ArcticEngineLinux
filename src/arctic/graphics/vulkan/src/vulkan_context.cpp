#include "arctic/graphics/vulkan/vulkan_context.h"

#include "vulkan_loader.h"
#include "vulkan_renderloop.h"
#include "arctic/graphics/vulkan/vulkan_window.h"

VulkanContext::VulkanContext(std::shared_ptr<VulkanWindow> vulkanWindow)
{
    pVulkanLoader = std::make_unique<VulkanLoader>(vulkanWindow);
}

VulkanContext::~VulkanContext()
{
    
}

void VulkanContext::Cleanup()
{
    // cleanup vulkan
    pVulkanLoader->Cleanup();
    pVulkanLoader.reset();
}

void VulkanContext::Render()
{
    // get renderloop
    auto renderLoop = pVulkanLoader->GetRenderLoop();

    // reload swapchain when dirty
    if(renderLoop->IsSwapChainDirty())
    {
        pVulkanLoader->ReloadSwapChain();
    }

    // render
    renderLoop->Render();
}