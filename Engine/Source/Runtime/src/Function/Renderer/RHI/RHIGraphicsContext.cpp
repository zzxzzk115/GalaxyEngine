#include "Function/Renderer/RHI/RHIGraphicsContext.h"
#include "Core/Macro.h"
#include "Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"

namespace Galaxy
{
    Scope<RHIGraphicsContext> RHIGraphicsContext::Create(void* window)
    {
        return CreateScope<VulkanGraphicsContext>(static_cast<GLFWwindow*>(window));
    }
} // namespace Galaxy