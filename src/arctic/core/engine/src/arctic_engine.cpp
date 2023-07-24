#include "arctic/core/engine/arctic_engine.h"
#include <SDL2/SDL.h>
#include "arctic/graphics/vulkan/vulkan_window.h"
#include "arctic/graphics/vulkan/vulkan_context.h"

ArcticEngine::ArcticEngine()
{
    
}

ArcticEngine::~ArcticEngine()
{
    
}

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
    pVulkanWindow = std::make_shared<VulkanWindow>();
    pVulkanWindow->CreateWindow();

    // load vulkan
    pVulkanContext = std::make_unique<VulkanContext>(pVulkanWindow);
}

void ArcticEngine::Cleanup()
{
    // cleanup vulkan
    pVulkanContext->Cleanup();
    pVulkanContext.reset();
    
    // cleanup window
    pVulkanWindow->CleanupWindow();
    pVulkanWindow.reset();
}

