//
// BuiltinRenderPipeline.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pipeline/BuiltinRenderPipeline.h"
#include "GalaxyEngine/Function/Renderer/Pass/GUIPass.h"
#include "GalaxyEngine/Function/Renderer/Pass/MainCameraPass.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"

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

        MainCameraPassInitInfo mainCameraInitInfo;
        m_MainCameraPass->Initialize(&mainCameraInitInfo);

        GUIPassInitInfo guiInitInfo;
        guiInitInfo.RenderPass = mainCameraPass->GetRenderPass();
        m_GUIPass->Initialize(&guiInitInfo);
    }

    void BuiltinRenderPipeline::ForwardRender(Ref<RHI> rhi)
    {
        VulkanRHI*      vulkanRhi      = static_cast<VulkanRHI*>(rhi.get());

        vulkanRhi->WaitForFences();

        vulkanRhi->ResetCommandPool();

        bool recreateSwapchain =
            vulkanRhi->PrepareBeforePass(std::bind(&BuiltinRenderPipeline::PassUpdateAfterRecreateSwapchain, this));
        if (recreateSwapchain)
        {
            return;
        }

        GUIPass&           guiPass            = *(static_cast<GUIPass*>(m_GUIPass.get()));

        vulkanRhi->SubmitRendering(std::bind(&BuiltinRenderPipeline::PassUpdateAfterRecreateSwapchain, this));
    }

    void BuiltinRenderPipeline::PassUpdateAfterRecreateSwapchain()
    {
        // TODO: update passes
    }
} // namespace Galaxy