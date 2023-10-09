#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class RHIGraphicsContext
    {
    public:
        virtual ~RHIGraphicsContext() = default;

        virtual void Init(void* window) = 0;

        virtual void Release() = 0;

        static Scope<RHIGraphicsContext> Create();
    };
} // namespace Galaxy