#pragma once

#include "GalaxyEngine/Function/Renderer/RenderSystem.h"

namespace Galaxy
{
    class VulkanRenderSystem : public RenderSystem
    {
    public:
        virtual void Init() override;
        virtual void Release() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    };
} // namespace Galaxy
