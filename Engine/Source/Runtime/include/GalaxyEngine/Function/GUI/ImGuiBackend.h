//
// ImGuiBackend.h
//
// Created or modified by Kexuan Zhang on 25/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/GUI/GUIBackend.h"

namespace Galaxy
{
    class ImGuiBackend : public GUIBackend
    {
    public:
        virtual void Initialize(GUIBackendInitInfo initInfo) override final;
        virtual void PreRender() override final;

    private:
        void SetDarkThemeColors();
    };
} // namespace Galaxy