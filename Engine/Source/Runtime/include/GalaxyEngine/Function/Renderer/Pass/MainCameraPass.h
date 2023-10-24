//
// MainCameraPass.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pass/RenderPass.h"

namespace Galaxy
{
    class MainCameraPass : public RenderPass
    {
    public:
        void Initialize(const RenderPassInitInfo* initInfo) override final;

    private:
        void SetupAttachments();
        void SetupRenderPass();
        void SetupDescriptorSetLayout();
        void SetupPipelines();
        void SetupDescriptorSet();
        void SetupFramebufferDescriptorSet();
        void SetupSwapchainFramebuffers();

    private:
        std::vector<RHIFramebuffer*> m_SwapchainFramebuffers;
    };
} // namespace Galaxy