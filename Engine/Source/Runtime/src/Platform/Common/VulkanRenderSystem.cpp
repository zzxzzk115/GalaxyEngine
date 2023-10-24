//
// VulkanRenderSystem.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Platform/Common/VulkanRenderSystem.h"

namespace Galaxy
{
    void VulkanRenderSystem::Init(RenderSystemInitInfo initInfo)
    {
        // 1. Init RHI
        RHIInitInfo rhiInitInfo = {};
        rhiInitInfo.WindowSys = initInfo.WindowSys;
        m_RHI = CreateRef<VulkanRHI>();
        m_RHI->Initialize(rhiInitInfo);
    }

    void VulkanRenderSystem::Release()
    {
        m_RHI.reset();
    }

    void VulkanRenderSystem::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {

    }
} // namespace Galaxy
