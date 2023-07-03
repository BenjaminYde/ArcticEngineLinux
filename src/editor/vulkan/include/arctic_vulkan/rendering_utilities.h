#pragma once

#include <vulkan/vulkan_core.h>
#include "arctic_rendering/vertex.h"
#include <array>

class RenderingUtilities
{
public:
    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};