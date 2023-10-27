//
// GUIBackend.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Function/Renderer/RHI/RHIStruct.h"

namespace Galaxy
{
    class WindowSystem;
    class RenderSystem;
    class RHI;

    struct GUIBackendInitInfo
    {
        Ref<WindowSystem> WindowSys;
        Ref<RenderSystem> RenderSys;
    };

    struct GUIBackendRenderingConfig
    {
        uint32_t        GUIPassIndex = 0;
        RHIRenderPass*  GUIRenderPass;
    };

    class GUIBackend
    {
    public:
        virtual void Initialize(GUIBackendInitInfo initInfo) = 0;
        virtual void SetupRenderingConfig(GUIBackendRenderingConfig config) = 0;
        virtual void Begin() = 0;
        virtual void PreRender() = 0;
        virtual void End() = 0;

    protected:
        Ref<RHI> m_RHI;
    };
} // namespace Galaxy