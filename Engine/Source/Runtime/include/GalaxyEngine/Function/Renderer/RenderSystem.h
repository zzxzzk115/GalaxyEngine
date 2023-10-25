//
// RenderSystem.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

namespace Galaxy
{
    class WindowSystem;
    class GUIBackend;

    struct RenderSystemInitInfo
    {
        Ref<WindowSystem> WindowSys;
    };

    class RenderSystem
    {
    public:
        virtual void     Init(RenderSystemInitInfo) = 0;
        virtual void     Update(float deltaTime)    = 0;
        virtual void     Release()                  = 0;
        virtual Ref<RHI> GetRHI()                   = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void InitializeUIRenderBackend(GUIBackend* guiBackend) = 0;
    };
} // namespace Galaxy
