#include "arctic_vulkan/vulkan_renderloop.h"

#include "arctic_vulkan/vulkan_swapchain.h"
#include "arctic_vulkan/vulkan_renderpipeline.h"
#include "arctic_vulkan/vulkan_memory_handler.h"
#include "arctic_rendering/vertex.h"
#include "arctic_rendering/uniform_buffer_object.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstring>
#include <chrono>

VulkanRenderLoop::VulkanRenderLoop(
    VkDevice vkDevice, 
    VulkanSwapChain* swapChain, 
    VulkanRenderPipeline* renderPipeline, 
    VulkanMemoryHandler* vkMemoryHandler, 
    VkQueue graphicsQueue, 
    VkQueue transferQueue, 
    VkQueue presentQueue)
    :
    vkDevice(vkDevice),
    pSwapchain(swapChain),
    pRenderPipeline(renderPipeline),
    vkMemoryHandler(vkMemoryHandler),
    vkGraphicsQueue(graphicsQueue),
    vkTransferQueue(transferQueue),
    vkPresentQueue(presentQueue)
{
    // define frames to use
    frames.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        this->frames[i] = new Frame();
        
    // create command pool and buffer
    createCommandPool(renderPipeline->GetGraphicsFamilyIndex(), renderPipeline->GetTransferFamilyIndex());
    createCommandBuffers();

    // syncing
    createSyncObjects();

    // create vertex buffer
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.25f}}
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    if(!createVertexBuffer(vertices))
    {
        std::cout << "error: vulkan: failed to create vertex buffer!";
        return;
    }

    if(!createIndexBuffer(indices))
    {
        std::cout << "error: vulkan: failed to create index buffer!";
        return;
    }

    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

void VulkanRenderLoop::CleanUp()
{
    // wait until device is not executing work
    vkDeviceWaitIdle(vkDevice);

    // syncing
    for(int i=0; i<MAX_FRAMES_IN_FLIGHT; ++i)
    {
        Frame* frame = this->frames[i];

        vkDestroySemaphore(vkDevice, frame->imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(vkDevice, frame->renderFinishedSemaphore, nullptr);
        vkDestroyFence(vkDevice, frame->isDoneRenderingFence, nullptr);
    }

    // command pool & buffer
    vkDestroyCommandPool(vkDevice, vkCommandPoolGraphics, nullptr);
    vkDestroyCommandPool(vkDevice, vkCommandPoolTransfer, nullptr);
    
    // buffers
    vkDestroyBuffer(vkDevice, vertexBuffer, nullptr);
    vkFreeMemory(vkDevice, vertexBufferMemory, nullptr);

    vkDestroyBuffer(vkDevice, indexBuffer, nullptr);
    vkFreeMemory(vkDevice, indexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {   
        Frame* frame = this->frames[i];

        vkDestroyBuffer(vkDevice, frame->uniformBuffer, nullptr);
        vkFreeMemory(vkDevice, frame->uniformBufferMemory, nullptr);
    }

    vkDestroyDescriptorPool(vkDevice, vkDescriptorPool, nullptr);
}

bool VulkanRenderLoop::IsSwapChainDirty() const
{
    return this->isSwapChainDirty;
}

void VulkanRenderLoop::Render()
{
    // wait until previous frame is finished
    //// todo: implement multiple frames in flight. this avoids idle time (cpu waiting for gpu & vice versa)
    //// https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
    Frame* frame = this->frames[currentFrameIndex];
    vkWaitForFences(vkDevice, 1, &frame->isDoneRenderingFence, VK_TRUE, UINT64_MAX);

    // acquire next image from swap chain

    // try acquire next image
    uint32_t availableImageIndex = 0;
    VkResult resultAcquireNextImage = vkAcquireNextImageKHR(vkDevice, pSwapchain->GetSwapChain(), UINT64_MAX, frame->imageAvailableSemaphore, VK_NULL_HANDLE, &availableImageIndex);

    // check if swapchain still up-to-date
    // >> recreate when not (due to window resizing, ...)
    if (resultAcquireNextImage == VK_ERROR_OUT_OF_DATE_KHR)
    {   
        // wait until idle
        vkDeviceWaitIdle(vkDevice);

        // mark dirty
        this->isSwapChainDirty = true;
        return;
    }
    else
    {
        this->isSwapChainDirty = false;
    }   

    // reset fence state to zero
    // only reset the fence if we are submitting work
    vkResetFences(vkDevice, 1, &frame->isDoneRenderingFence);
    
    // record command buffer
    vkResetCommandBuffer(frame->commandBuffer, 0);
    recordCommandBuffer(*frame, availableImageIndex);

    // create info: command buffer submit 
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //> specify semaphores to wait on before execution
    VkSemaphore waitSemaphores[] = { frame->imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    //> specify command buffer
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame->commandBuffer;

    //> signal semaphores on finish execution
    VkSemaphore signalSemaphores[] = { frame->renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // submit command buffer to graphics queue
    VkResult resultQueueSubmit = vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, frame->isDoneRenderingFence);
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

    // increase current image frame
    currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderLoop::createCommandPool(uint32_t graphicsFamilyIndex, uint32_t transferFamilyIndex)
{
    // create info: command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // we record a command buffer every frame, so we want to be able to reset and re-record
    poolInfo.queueFamilyIndex = graphicsFamilyIndex;

    // create command pool
    VkResult result = vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &vkCommandPoolGraphics);
    if (result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create command pool!";
        return;
    }

    // create info: command pool
    VkCommandPoolCreateInfo poolInfo2{};
    poolInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo2.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo2.queueFamilyIndex = transferFamilyIndex;

    // create command pool
    result = vkCreateCommandPool(vkDevice, &poolInfo2, nullptr, &vkCommandPoolTransfer);
    if (result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create command pool!";
        return;
    }
}

void VulkanRenderLoop::createCommandBuffers()
{
    // create all buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {   
        // get frame
        Frame* frame = this->frames[i];

        // create info: command buffer allocation
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vkCommandPoolGraphics;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can be submitted to a queue for execution, but cannot be called from other command buffers
        allocInfo.commandBufferCount = 1;

        // create command buffers
        //this->vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkResult result = vkAllocateCommandBuffers(vkDevice, &allocInfo, &frame->commandBuffer);
        if (result != VK_SUCCESS)
        {
            std::cout << "error: vulkan: failed to create command buffer!";
            return;
        }
    }
}

bool VulkanRenderLoop::createVertexBuffer(std::vector<Vertex> vertices)
{   
    bool useStagingBuffer = true;
    if(useStagingBuffer)
    {
        VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

        VkBuffer vertexBufferStaging;
        VkDeviceMemory vertexBufferMemoryStaging;

        // create vertex buffer: staging (cpu interaction)
        // >> Buffer can be used as source in a memory transfer operation
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryProperties, vertexBufferStaging, vertexBufferMemoryStaging))
                return false;
        
            // copy vertices to memory
            void* data;
            vkMapMemory(this->vkDevice, vertexBufferMemoryStaging, 0, bufferSize, 0, &data);
                memcpy(data, vertices.data(), (size_t) bufferSize);
            vkUnmapMemory(this->vkDevice, vertexBufferMemoryStaging);
        }

        // create vertex buffer: final (gpu interaction)
        // >> Buffer can be used as destination in a memory transfer operation
        // >> The finalvertexBuffer is allocated from a memory type that is device local, which generally means that we're not able to use vkMapMemory
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryProperties, this->vertexBuffer, this->vertexBufferMemory))
                return false;
        }

        // copy vertex buffer data from stating to final
        vkMemoryHandler->CopyBuffer(vertexBufferStaging, this->vertexBuffer, bufferSize, this->vkCommandPoolTransfer);

        // cleanup vertex buffer staging (not used anymore)
        vkDestroyBuffer(this->vkDevice, vertexBufferStaging, nullptr);
        vkFreeMemory(this->vkDevice, vertexBufferMemoryStaging, nullptr);
    }
    else
    {
        VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        VkMemoryPropertyFlags memoryPropeties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryPropeties, this->vertexBuffer, this->vertexBufferMemory))
            return false;

        void* data;
        vkMapMemory(this->vkDevice, this->vertexBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(this->vkDevice, this->vertexBufferMemory);
    }

    return true;
}

bool VulkanRenderLoop::createIndexBuffer(std::vector<uint32_t> indices)
{
    bool useStagingBuffer = true;
    if(useStagingBuffer)
    {
        VkDeviceSize bufferSize = sizeof(Vertex) * indices.size();

        VkBuffer indexBufferStaging;
        VkDeviceMemory indexBufferMemoryStaging;

        // create index buffer: staging (cpu interaction)
        // >> Buffer can be used as source in a memory transfer operation
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

            if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryProperties, indexBufferStaging, indexBufferMemoryStaging))
                return false;
        
            // copy indices to memory
            void* data;
            vkMapMemory(this->vkDevice, indexBufferMemoryStaging, 0, bufferSize, 0, &data);
                memcpy(data, indices.data(), (size_t) bufferSize);
            vkUnmapMemory(this->vkDevice, indexBufferMemoryStaging);
        }

        // create index buffer: final (gpu interaction)
        // >> Buffer can be used as destination in a memory transfer operation
        // >> The finalindexBuffer is allocated from a memory type that is device local, which generally means that we're not able to use vkMapMemory
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryProperties, this->indexBuffer, this->indexBufferMemory))
                return false;
        }

        // copy index buffer data from stating to final
        vkMemoryHandler->CopyBuffer(indexBufferStaging, this->indexBuffer, bufferSize, this->vkCommandPoolTransfer);

        // cleanup index buffer staging (not used anymore)
        vkDestroyBuffer(this->vkDevice, indexBufferStaging, nullptr);
        vkFreeMemory(this->vkDevice, indexBufferMemoryStaging, nullptr);
    }
    else
    {
        VkDeviceSize bufferSize = sizeof(Vertex) * indices.size();
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        VkMemoryPropertyFlags memoryPropeties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryPropeties, this->indexBuffer, this->indexBufferMemory))
            return false;

        void* data;
        vkMapMemory(this->vkDevice, this->indexBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t) bufferSize);
        vkUnmapMemory(this->vkDevice, this->indexBufferMemory);
    }
    return true;
}

/// @brief Create multiple uniform buffers.     
/// @brief We're going to copy new data to the uniform buffer every frame, it doesn't really make any sense to have a staging buffer.       
/// @brief We should have multiple buffers, because multiple frames may be in flight at the same time and we don't want to update       
/// @brief the buffer in preparation of the next frame while a previous one is still reading from it. Thus, we need to have as many         
/// @brief uniform buffers as we have frames in flight, and write to a uniform buffer that is not currently being read by the GPU
/// @return true when creation was successful 
bool VulkanRenderLoop::createUniformBuffers()
{   
    // resize buffers
    //uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    //uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    //uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    // create all buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {   
        // get frame
        Frame* frame = this->frames[i];

        // define buffer    
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        // create buffer
        if(!vkMemoryHandler->CreateBuffer(bufferSize, usage, memoryFlags, frame->uniformBuffer, frame->uniformBufferMemory))
            return false;

        // map memory to buffer
        vkMapMemory(this->vkDevice, frame->uniformBufferMemory, 0, bufferSize, 0, &frame->uniformBufferMapped);
    }
    return true;
}

void VulkanRenderLoop::updateUniformBuffer(const Frame& frame)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    auto swapchainData = pSwapchain->GetData();
    auto swapchainExtent = swapchainData.extent;

    ubo.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(frame.uniformBufferMapped, &ubo, sizeof(ubo));
}

bool VulkanRenderLoop::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkResult result = vkCreateDescriptorPool(this->vkDevice, &poolInfo, nullptr, &this->vkDescriptorPool);
    if (result != VK_SUCCESS)
    {
        std::cout << "error: vulkan: failed to create descriptor pool!";
        return false;
    }

    return true;
}

bool VulkanRenderLoop::createDescriptorSets()
{
    // allocate 
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        // get frame
        Frame* frame = this->frames[i];

        // create
        auto descriptorSetLayout = this->pRenderPipeline->GetDescriptorSetLayout();
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = this->vkDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        //this->vkDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        VkResult result = vkAllocateDescriptorSets(this->vkDevice, &allocInfo, &frame->descriptorSet);
        if (result != VK_SUCCESS)
        {
            std::cout << "error: vulkan: failed to create descriptor sets!";
            return false;
        }

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = frame->uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = frame->descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(this->vkDevice, 1, &descriptorWrite, 0, nullptr);
    }
    return true;
}

void VulkanRenderLoop::recordCommandBuffer(const Frame& frame, uint32_t imageIndex)
{
    // get command buffer
    VkCommandBuffer commandBuffer = frame.commandBuffer;

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

    // command buffer: bind vertex buffer   
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // command buffer: bind index buffer
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // command buffer: bind descriptor sets
    updateUniformBuffer(frame);
    auto pipelineLayout = pRenderPipeline->GetPipelineLayout();
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &frame.descriptorSet, 0, nullptr);

    // command buffer: draw
    uint32_t vertexCount = 3;
    //vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

    uint32_t indexCount = 6;
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    
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

void VulkanRenderLoop::createSyncObjects()
{   
    // create info
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // is signaled on start

    // resize memory
    //imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    //renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    //isDoneRenderingFence.resize(MAX_FRAMES_IN_FLIGHT);

    for(int i=0; i<MAX_FRAMES_IN_FLIGHT; ++i)
    {
        // get frame
        Frame* frame = this->frames[i];

        // create objects
        if (vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &frame->imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &frame->renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(vkDevice, &fenceInfo, nullptr, &frame->isDoneRenderingFence) != VK_SUCCESS)
        {
            std::cout << "error: vulkan: failed to create sync objects!";
            return;
        }
    }
}