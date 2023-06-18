#include "arctic_vulkan/vulkan_renderloop.h"
#include <iostream>
#include "arctic_vulkan/vulkan_swapchain.h"
#include "arctic_vulkan/vulkan_renderpipeline.h"

VulkanRenderLoop::VulkanRenderLoop(
    VkDevice vkDevice, 
    VulkanSwapChain* swapChain, 
    VulkanRenderPipeline* renderPipeline, 
    VkQueue graphicsQueue, 
    VkQueue presentQueue)
    :
    vkDevice(vkDevice),
    pSwapchain(swapChain),
    pRenderPipeline(renderPipeline),
    vkGraphicsQueue(graphicsQueue),
    vkPresentQueue(presentQueue)
{
    // create command pool and buffer
    vulkanCreateCommandPool(renderPipeline->GetGraphicsFamilyIndex());
    vulkanCreateCommandBuffer();

    // syncing
    vulkanCreateSyncObjects();
}

void VulkanRenderLoop::CleanUp()
{
    // wait until device is not executing work
    vkDeviceWaitIdle(vkDevice);

    // syncing
    vkDestroySemaphore(vkDevice, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, renderFinishedSemaphore, nullptr);
    vkDestroyFence(vkDevice, isDoneRenderingFence, nullptr);

    // command pool & buffer
    vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
}

void VulkanRenderLoop::Render()
{
    // wait until previous frame is finished
    //// todo: implement multiple frames in flight. this avoids idle time (cpu waiting for gpu & vice versa)
    //// https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
    vkWaitForFences(vkDevice, 1, &isDoneRenderingFence, VK_TRUE, UINT64_MAX);

    // acquire next image from swap chain
    uint32_t availableImageIndex;
    VkResult resultAcquireNextImage = vkAcquireNextImageKHR(vkDevice, pSwapchain->GetSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &availableImageIndex);

    // check if swapchain still up-to-date
    // >> recreate when not (due to window resizing, ...)
    if (resultAcquireNextImage == VK_ERROR_OUT_OF_DATE_KHR)
    {   
        // wait until idle
        vkDeviceWaitIdle(vkDevice);

        // re-create swapchain and re-load dependencies on swapchain data
        pSwapchain->CleanUp(vkDevice);
        pSwapchain->CreateSwapChain();

        pRenderPipeline->Load(
            pSwapchain->GetData(), 
            pSwapchain->GetImageViews());
        
        return;
    }

    // reset fence state to zero
    // only reset the fence if we are submitting work
    vkResetFences(vkDevice, 1, &isDoneRenderingFence);
    
    // record command buffer
    vkResetCommandBuffer(vkCommandBuffer, 0);
    vulkanRecordCommandBuffer(vkCommandBuffer, availableImageIndex);

    // create info: command buffer submit 
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //> specify semaphores to wait on before execution
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    //> specify command buffer
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    //> signal semaphores on finish execution
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // submit command buffer to graphics queue
    VkResult resultQueueSubmit = vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, isDoneRenderingFence);
    if(resultQueueSubmit != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to submit command buffer to graphics queue!";
        return;
    }

    // create info: present khr
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { pSwapchain->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &availableImageIndex;
    presentInfo.pResults = nullptr; // Optional

    // queue present khr
    vkQueuePresentKHR(vkPresentQueue, &presentInfo);
}

void VulkanRenderLoop::vulkanCreateCommandPool(uint32_t graphicsFamilyIndex)
{
    // create info: command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // we record a command buffer every frame, so we want to be able to reset and re-record
    poolInfo.queueFamilyIndex = graphicsFamilyIndex;

    // create command pool
    VkResult result = vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &vkCommandPool);
    if (result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create command pool!";
        return;
    }
}

void VulkanRenderLoop::vulkanCreateCommandBuffer()
{
    // create info: command buffer allocation
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can be submitted to a queue for execution, but cannot be called from other command buffers
    allocInfo.commandBufferCount = 1;

    // create command buffer
    VkResult result = vkAllocateCommandBuffers(vkDevice, &allocInfo, &vkCommandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create command buffer!";
        return;
    }
}

void VulkanRenderLoop::vulkanRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // command buffer: begin
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // optional
    beginInfo.pInheritanceInfo = nullptr; // optional

    VkResult resultBeginCommandBuffer = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (resultBeginCommandBuffer != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to begin command buffer!";
        return;
    }

    // get swapchain data
    SwapChainData swapChainData = pSwapchain->GetData();

    // command buffer: begin render pass
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = pRenderPipeline->GetRenderPass();
    renderPassBeginInfo.framebuffer = pRenderPipeline->GetFrameBuffer(imageIndex);

    renderPassBeginInfo.renderArea.offset = VkOffset2D {0, 0};
    renderPassBeginInfo.renderArea.extent = swapChainData.extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // command buffer: bind to pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pRenderPipeline->GetPipeline());

    // command buffer: set viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainData.extent.width;
    viewport.height = (float) swapChainData.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // command buffer: set scissor
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainData.extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // command buffer: draw
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // command buffer: end render pass
    vkCmdEndRenderPass(commandBuffer);

    // command buffer: end
    VkResult resultEndCommandBuffer = vkEndCommandBuffer(commandBuffer);
    if (resultEndCommandBuffer != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to end command buffer!";
        return;
    }
}

void VulkanRenderLoop::vulkanCreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // is signaled on start

    if (vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(vkDevice, &fenceInfo, nullptr, &isDoneRenderingFence) != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create sync objects!";
        return;
    }
}