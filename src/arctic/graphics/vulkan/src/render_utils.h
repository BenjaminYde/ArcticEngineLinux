#pragma once

#include <vulkan/vulkan_core.h>
#include "arctic/graphics/rhi/vertex.h"
#include <array>

class RenderUtils
{
public:
    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};