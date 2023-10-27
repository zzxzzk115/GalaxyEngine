//
// RenderPipeline.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Core/Math/Math.h"
#include "GalaxyEngine/Function/Renderer/Pass/RenderPassBase.h"

using namespace Galaxy::Math;

namespace Galaxy
{
    class RHI;
    class GUIBackend;

    struct RenderPipelineInitInfo {};

    class RenderPipelineBase
    {
        friend class VulkanRenderSystem;

    public:
        virtual ~RenderPipelineBase() = default;

        virtual void Clear() {};
        virtual void Initialize(RenderPipelineInitInfo initInfo) = 0;

        void InitializeUIRenderBackend(GUIBackend* guiBackend);

        virtual void ForwardRender();

    protected:
        Ref<RHI> m_RHI;

        Ref<RenderPassBase> m_MainCameraPass;
        Ref<RenderPassBase> m_GUIPass;
    };
} // namespace Galaxy