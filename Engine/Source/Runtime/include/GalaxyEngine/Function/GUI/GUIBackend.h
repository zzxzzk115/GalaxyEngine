//
// GUIBackend.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class WindowSystem;
    class RenderSystem;

    struct GUIBackendInitInfo
    {
        Ref<WindowSystem> WindowSys;
        Ref<RenderSystem> RenderSys;
    };

    class GUIBackend
    {
    public:
        virtual void Initialize(GUIBackendInitInfo initInfo) = 0;
        virtual void PreRender() = 0;
    };
} // namespace Galaxy