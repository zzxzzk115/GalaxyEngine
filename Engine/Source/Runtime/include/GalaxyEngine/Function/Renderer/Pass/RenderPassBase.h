//
// RenderPassBase.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

namespace Galaxy
{
    class RHI;
    class RenderResourceBase;
    class GUIBackend;

    struct RenderPassInitInfo
    {};

    struct RenderPassCommonInfo
    {
        Ref<RHI>                RHI;
        Ref<RenderResourceBase> RenderResource;
    };

    class RenderPassBase
    {
    public:
        virtual void Initialize(const RenderPassInitInfo* initInfo) = 0;
        virtual void PostInitialize();
        virtual void SetCommonInfo(RenderPassCommonInfo commonInfo);
        virtual void InitializeUIRenderBackend(GUIBackend* guiBackend);

    protected:
        Ref<RHI>                m_RHI;
        Ref<RenderResourceBase> m_RenderResource;
    };
} // namespace Galaxy