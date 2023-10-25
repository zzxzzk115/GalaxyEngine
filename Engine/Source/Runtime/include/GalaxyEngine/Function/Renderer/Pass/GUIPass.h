//
// GUIPass.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pass/RenderPass.h"

namespace Galaxy
{
    struct GUIPassInitInfo : RenderPassInitInfo
    {
        RHIRenderPass* RenderPass;
    };

    class GUIPass : public RenderPass
    {
    public:
        void Initialize(const RenderPassInitInfo* initInfo) override final;
        void InitializeUIRenderBackend(GUIBackend* guiBackend) override final;
        void Draw() override final;

    private:
        void UploadFonts();

    private:
        GUIBackend* m_GUIBackend;
    };
} // namespace Galaxy