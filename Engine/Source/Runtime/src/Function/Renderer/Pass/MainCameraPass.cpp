//
// MainCameraPass.cpp
//
// Created or modified by Kexuan Zhang on 25/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/MainCameraPass.h"

namespace Galaxy
{
    void MainCameraPass::Initialize(const RenderPassInitInfo* initInfo)
    {
        RenderPass::Initialize(nullptr);

        SetupAttachments();
        SetupRenderPass();
        SetupDescriptorSetLayout();
        SetupPipelines();
        SetupDescriptorSet();
        SetupFramebufferDescriptorSet();
        SetupSwapchainFramebuffers();
    }

    void MainCameraPass::SetupAttachments() {}

    void MainCameraPass::SetupRenderPass() {}

    void MainCameraPass::SetupDescriptorSetLayout() {}

    void MainCameraPass::SetupPipelines() {}

    void MainCameraPass::SetupDescriptorSet() {}

    void MainCameraPass::SetupFramebufferDescriptorSet() {}

    void MainCameraPass::SetupSwapchainFramebuffers() {}
} // namespace Galaxy