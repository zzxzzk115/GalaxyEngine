//
// RenderResourceBase.h
//
// Created or modified by Kexuan Zhang on 27/10/2023.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class RHI;

    class RenderResourceBase
    {
    public:
        virtual ~RenderResourceBase() = default;

        virtual void Clear() = 0;

        virtual void UploadGlobalRenderResource(const Ref<RHI>& rhi) = 0;
    };
} // namespace Galaxy