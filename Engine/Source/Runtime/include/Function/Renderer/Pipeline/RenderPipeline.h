#pragma once

#include "Core/Base.h"

namespace Galaxy
{
    enum class RenderPipelineType
    {
        Forward = 0,
        Deferred
    };

    class RenderPipeline
    {
    public:
        virtual ~RenderPipeline() = default;

        virtual void Init()     = 0;
        virtual void Shutdown() = 0;

        virtual void ForwardRender()  = 0;
        virtual void DeferredRender() = 0;

    public:
        static Scope<RenderPipeline> Create();
    };
} // namespace Galaxy