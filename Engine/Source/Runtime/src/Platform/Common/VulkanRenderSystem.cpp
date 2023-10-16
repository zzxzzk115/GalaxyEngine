//
// VulkanRenderSystem.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Platform/Common/VulkanRenderSystem.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"

#include <vulkan/vulkan.h>

namespace Galaxy
{
    void VulkanRenderSystem::Init() {}

    void VulkanRenderSystem::Release() {}

    void VulkanRenderSystem::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        g_VulkanGraphicsContextPtr->FrameBufferResized = true;
    }
} // namespace Galaxy
