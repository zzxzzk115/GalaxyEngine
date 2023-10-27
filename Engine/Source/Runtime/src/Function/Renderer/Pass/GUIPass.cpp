//
// GUIPass.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/GUIPass.h"
#include "GalaxyEngine/Function/GUI/GUIBackend.h"

namespace Galaxy
{
    void GUIPass::Initialize(const RenderPassInitInfo* initInfo)
    {
        RenderPass::Initialize(nullptr);
        Framebuffer.RenderPass = static_cast<const GUIPassInitInfo*>(initInfo)->RenderPass;
    }

    void GUIPass::InitializeUIRenderBackend(GUIBackend* guiBackend)
    {
        m_GUIBackend = guiBackend;

        if (m_GUIBackend)
        {
            GUIBackendRenderingConfig config;
            config.GUIRenderPass = Framebuffer.RenderPass;
            config.GUIPassIndex = _main_camera_subpass_gui;
            m_GUIBackend->SetupRenderingConfig(config);
        }
    }

    void GUIPass::Draw()
    {
        if (m_GUIBackend)
        {
            m_GUIBackend->Begin();
            m_GUIBackend->PreRender();
            m_GUIBackend->End();
        }
    }
} // namespace Galaxy