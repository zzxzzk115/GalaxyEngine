//
// BuiltinRenderPipeline.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pipeline/RenderPipeline.h"

namespace Galaxy
{
    class BuiltinRenderPipeline : public RenderPipeline
    {
    public:
        virtual void Init() override;
        virtual void Shutdown() override;

        virtual void ForwardRender() override;
        virtual void DeferredRender() override;
    };
} // namespace Galaxy
