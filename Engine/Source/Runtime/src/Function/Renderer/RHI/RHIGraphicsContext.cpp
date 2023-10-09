#include "GalaxyEngine/Function/Renderer/RHI/RHIGraphicsContext.h"
#include "GalaxyEngine/Core/Macro.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"

namespace Galaxy
{
    Scope<RHIGraphicsContext> RHIGraphicsContext::Create()
    {
        return CreateScope<VulkanGraphicsContext>();
    }
} // namespace Galaxy