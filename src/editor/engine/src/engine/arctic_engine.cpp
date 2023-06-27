#include "engine/arctic_engine.h"
#include <SDL2/SDL.h>
#include "arctic_vulkan/vulkan_loader.h"
#include "arctic_vulkan/vulkan_renderloop.h"
#include "arctic_vulkan/vulkan_window.h"

void ArcticEngine::Run()
{
    // loop while no close window
    auto window = pVulkanWindow->GetWindow();
    SDL_Event event;
    bool running = true;
    while(running)
    {
        // check input
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }
        
        // render
        VulkanRenderLoop renderLoop = vulkanLoader->GetRenderLoop();
        renderLoop.Render();
    }
}

void ArcticEngine::Initialize()
{
    // create window
    pVulkanWindow = new VulkanWindow();
    pVulkanWindow->CreateWindow();

    // load vulkan
    vulkanLoader = new VulkanLoader(*pVulkanWindow);
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

