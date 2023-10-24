//
// BuiltinRenderPipeline.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pipeline/RenderPipelineBase.h"

namespace Galaxy
{
    class BuiltinRenderPipeline : public RenderPipelineBase
    {
    public:
        virtual void Initialize(RenderPipelineInitInfo initInfo) override final;
    };
} // namespace Galaxy
