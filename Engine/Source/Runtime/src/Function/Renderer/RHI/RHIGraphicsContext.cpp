//
// RHIGraphicsContext.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Function/Renderer/RHI/RHIGraphicsContext.h"
#include "GalaxyEngine/Core/Macro.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"

namespace Galaxy
{
    Scope<RHIGraphicsContext> RHIGraphicsContext::Create() { return CreateScope<VulkanGraphicsContext>(); }
} // namespace Galaxy