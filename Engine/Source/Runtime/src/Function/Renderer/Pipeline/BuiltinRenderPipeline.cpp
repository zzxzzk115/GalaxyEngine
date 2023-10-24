//
// BuiltinRenderPipeline.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pipeline/BuiltinRenderPipeline.h"
#include "GalaxyEngine/Function/Renderer/Pass/MainCameraPass.h"
#include "GalaxyEngine/Function/Renderer/Pass/GUIPass.h"

namespace Galaxy
{
    void BuiltinRenderPipeline::Initialize(RenderPipelineInitInfo  /*initInfo*/)
    {
        m_MainCameraPass = CreateRef<MainCameraPass>();
        m_GUIPass = CreateRef<GUIPass>();

        RenderPassCommonInfo passCommonInfo;
        passCommonInfo.RHI = m_RHI;

        m_MainCameraPass->SetCommonInfo(passCommonInfo);
        m_GUIPass->SetCommonInfo(passCommonInfo);

        auto mainCameraPass = std::static_pointer_cast<MainCameraPass>(m_MainCameraPass);

        GUIPassInitInfo guiInitInfo;
        guiInitInfo.RenderPass = mainCameraPass->GetRenderPass();
        m_GUIPass->Initialize(&guiInitInfo);
    }
} // namespace Galaxy