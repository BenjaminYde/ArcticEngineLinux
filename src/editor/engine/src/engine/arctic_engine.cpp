#include "engine/arctic_engine.h"
#include <SDL2/SDL.h>
#include "arctic_vulkan/vulkan_window.h"
#include "arctic_vulkan/vulkan_context.h"

void ArcticEngine::Run()
{
    // loop while no close window
    auto window = pVulkanWindow->GetSDLWindow();
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
        pVulkanContext->Render();
    }
}

void ArcticEngine::Initialize()
{
    // create window
    pVulkanWindow = new VulkanWindow();
    pVulkanWindow->CreateWindow();

    // load vulkan
    pVulkanContext = new VulkanContext(pVulkanWindow);

    // // define 3d data
    // const std::vector<Vertex> vertices = {
    //     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    // };
}

void ArcticEngine::Cleanup()
{
    // cleanup vulkan
    pVulkanContext->Cleanup();

    // cleanup window
    pVulkanWindow->CleanupWindow();
    delete pVulkanWindow;
}

