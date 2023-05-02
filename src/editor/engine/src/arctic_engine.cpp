#include <GLFW/glfw3.h>
#include "vulkan_loader.h"
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
        vulkanLoader->Draw();
    }
}

void ArcticEngine::Initialize()
{
    // create window
    pVulkanWindow = new VulkanWindow();
    pVulkanWindow->CreateWindow();

    // load vulkan
    vulkanLoader = new VulkanLoader();
    vulkanLoader->Load(*pVulkanWindow);
}

void ArcticEngine::Cleanup()
{
    // cleanup vulkan
    vulkanLoader->Cleanup();
    delete vulkanLoader;

    // cleanup window
    pVulkanWindow->CleanupWindow();
    delete pVulkanWindow;
}
