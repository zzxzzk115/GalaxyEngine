//
// RenderPass.h
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/Pass/RenderPassBase.h"

namespace Galaxy
{
    enum
    {
        _main_camera_pass_base_color = 0,
        _main_camera_pass_backup_buffer_odd = 1,
        _main_camera_pass_backup_buffer_even = 2,
        _main_camera_pass_depth = 3,
        _main_camera_pass_custom_attachment_count = 4,
        _main_camera_pass_total_attachment_count = 4,
    };

    enum
    {
        _main_camera_subpass_basepass = 0,
        _main_camera_subpass_gui,
        _main_camera_subpass_count
    };

    class RenderPass : public RenderPassBase
    {
    public:
        struct FrameBufferAttachment
        {
            RHIImage*        Image;
            RHIDeviceMemory* Mem;
            RHIImageView*    View;
            RHIFormat        Format;
        };

        struct Framebuffer
        {
            int             Width;
            int             Height;
            RHIFramebuffer* Framebuffer;
            RHIRenderPass*  RenderPass;

            std::vector<FrameBufferAttachment> Attachments;
        };

        struct Descriptor
        {
            RHIDescriptorSetLayout* Layout;
            RHIDescriptorSet*       DescriptorSet;
        };

        struct RenderPipelineBase
        {
            RHIPipelineLayout* Layout;
            RHIPipeline*       Pipeline;
        };

        std::vector<Descriptor>         DescriptorInfos;
        std::vector<RenderPipelineBase> RenderPipelines;
        Framebuffer                     Framebuffer;

        void Initialize(const RenderPassInitInfo* initInfo) override;
        void PostInitialize() override;

        virtual void Draw();

        virtual RHIRenderPass*                       GetRenderPass() const;
        virtual std::vector<RHIImageView*>           GetFramebufferImageViews() const;
        virtual std::vector<RHIDescriptorSetLayout*> GetDescriptorSetLayouts() const;
    };
} // namespace Galaxy