//
// VulkanRenderSystem.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RenderSystem.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"

namespace Galaxy
{
    class VulkanRenderSystem : public RenderSystem
    {
    public:
        virtual void Init(RenderSystemInitInfo initInfo) override;
        virtual void Release() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    private:
        Ref<VulkanRHI> m_RHI;
    };
} // namespace Galaxy
