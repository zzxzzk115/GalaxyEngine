//
// RenderSystem.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class RenderSystem
    {
    public:
        virtual void Init()    = 0;
        virtual void Release() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    };
} // namespace Galaxy
