//
// MainCameraPass.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pass/GUIPass.h"
#include "GalaxyEngine/Function/Renderer/Pass/RenderPass.h"

namespace Galaxy
{
    enum LayoutType : uint8_t
    {
        _per_mesh = 0,
        _mesh_global,
        _layout_type_count
    };

    enum RenderPipeLineType : uint8_t
    {
        _render_pipeline_type_mesh_gbuffer = 0,
        _render_pipeline_type_count
    };

    struct MainCameraPassInitInfo : RenderPassInitInfo {};

    class MainCameraPass : public RenderPass
    {
    public:
        void Initialize(const RenderPassInitInfo* initInfo) override final;

        void DrawForward(GUIPass& guiPass,
                         uint32_t currentSwapchainImageIndex);

    private:
        void SetupAttachments();
        void SetupRenderPass();
        void SetupDescriptorSetLayout();
        void SetupPipelines();
        void SetupDescriptorSet();
        void SetupFramebufferDescriptorSet();
        void SetupSwapchainFramebuffers();

        void SetupModelDescriptorSet();

    private:
        std::vector<RHIFramebuffer*> m_SwapchainFramebuffers;
    };
} // namespace Galaxy