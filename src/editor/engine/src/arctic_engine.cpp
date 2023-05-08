#include <GLFW/glfw3.h>
#include "vulkan_loader.h"
#include "renderloop.h"
#include "vulkan_window.h"
#include "engine/arctic_engine.h"

void ArcticEngine::Run()
{
    // loop while no close window
    auto window = pVulkanWindow->GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        // check input
        glfwPollEvents();

        // draw
        pRenderLoop->Draw();
    }
}

void ArcticEngine::Initialize()
{
    // create window
    pVulkanWindow = new VulkanWindow();
    pVulkanWindow->CreateWindow();

    // load vulkan
    // todo: move load to constructor
    // todo: move load to constructor
    // todo: move load to constructor
    vulkanLoader = new VulkanLoader();
    pRenderLoop = nullptr;
    vulkanLoader->Load(*pVulkanWindow, pRenderLoop);
}

void ArcticEngine::Cleanup()
{
    // cleanup vulkan
    pRenderLoop->CleanUp();
    delete pRenderLoop;

    vulkanLoader->Cleanup();
    delete vulkanLoader;

    // cleanup window
    pVulkanWindow->CleanupWindow();
    delete pVulkanWindow;
}

