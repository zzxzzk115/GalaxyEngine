#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class RHIGraphicsContext
    {
    public:
        virtual ~RHIGraphicsContext() = default;

        virtual void Init() = 0;

        virtual void Release() = 0;

        static Scope<RHIGraphicsContext> Create(void* window);
    };
} // namespace Galaxy