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

    void MainCameraPass::DrawForward(GUIPass& guiPass, uint32_t currentSwapchainImageIndex)
    {
        // TODO: draw passes
    }

    void MainCameraPass::SetupAttachments()
    {
        // TODO: add more framebuffer attachments, e.g. GBuffer, PostProcessingBuffer...
        Framebuffer.Attachments.resize(_main_camera_pass_custom_attachment_count);

        Framebuffer.Attachments[_main_camera_pass_base_color].Format = RHI_FORMAT_R8G8B8A8_SRGB;
        Framebuffer.Attachments[_main_camera_pass_depth].Format = RHI_FORMAT_D16_UNORM;
        Framebuffer.Attachments[_main_camera_pass_backup_buffer_odd].Format  = RHI_FORMAT_R16G16B16A16_SFLOAT;
        Framebuffer.Attachments[_main_camera_pass_backup_buffer_even].Format = RHI_FORMAT_R16G16B16A16_SFLOAT;

        for (uint32_t attachmentIndex = 0; attachmentIndex < _main_camera_pass_custom_attachment_count; ++attachmentIndex)
        {
            if (attachmentIndex == _main_camera_pass_depth)
            {
                m_RHI->CreateImage(m_RHI->GetSwapchainInfo().extent.width,
                                   m_RHI->GetSwapchainInfo().extent.height,
                                   Framebuffer.Attachments[_main_camera_pass_depth].Format,
                                   RHI_IMAGE_TILING_OPTIMAL,
                                   RHI_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                   RHI_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                   Framebuffer.Attachments[_main_camera_pass_depth].Image,
                                   Framebuffer.Attachments[_main_camera_pass_depth].Mem,
                                   0,
                                   1,
                                   1);

                m_RHI->CreateImageView(Framebuffer.Attachments[_main_camera_pass_depth].Image,
                                       Framebuffer.Attachments[_main_camera_pass_depth].Format,
                                       RHI_IMAGE_ASPECT_DEPTH_BIT,
                                       RHI_IMAGE_VIEW_TYPE_2D,
                                       1,
                                       1,
                                       Framebuffer.Attachments[_main_camera_pass_depth].View);
            }
            else
            {
                m_RHI->CreateImage(m_RHI->GetSwapchainInfo().extent.width,
                                   m_RHI->GetSwapchainInfo().extent.height,
                                   Framebuffer.Attachments[attachmentIndex].Format,
                                   RHI_IMAGE_TILING_OPTIMAL,
                                   RHI_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | RHI_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                       RHI_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                                   RHI_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                   Framebuffer.Attachments[attachmentIndex].Image,
                                   Framebuffer.Attachments[attachmentIndex].Mem,
                                   0,
                                   1,
                                   1);

                m_RHI->CreateImageView(Framebuffer.Attachments[attachmentIndex].Image,
                                       Framebuffer.Attachments[attachmentIndex].Format,
                                       RHI_IMAGE_ASPECT_COLOR_BIT,
                                       RHI_IMAGE_VIEW_TYPE_2D,
                                       1,
                                       1,
                                       Framebuffer.Attachments[attachmentIndex].View);
            }
        }
    }

    void MainCameraPass::SetupRenderPass()
    {
        RHIAttachmentDescription attachments[_main_camera_pass_total_attachment_count] = {};

        RHIAttachmentDescription& colorAttachmentDescription = attachments[_main_camera_pass_base_color];
        colorAttachmentDescription.format  = Framebuffer.Attachments[_main_camera_pass_base_color].Format;
        colorAttachmentDescription.samples = RHI_SAMPLE_COUNT_1_BIT;
        colorAttachmentDescription.loadOp  = RHI_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDescription.storeOp = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.stencilLoadOp  = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.initialLayout  = RHI_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout    = RHI_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        RHIAttachmentDescription& backupOddColorAttachmentDescription =
            attachments[_main_camera_pass_backup_buffer_odd];
        backupOddColorAttachmentDescription.format =
            Framebuffer.Attachments[_main_camera_pass_backup_buffer_odd].Format;
        backupOddColorAttachmentDescription.samples        = RHI_SAMPLE_COUNT_1_BIT;
        backupOddColorAttachmentDescription.loadOp         = RHI_ATTACHMENT_LOAD_OP_CLEAR;
        backupOddColorAttachmentDescription.storeOp        = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        backupOddColorAttachmentDescription.stencilLoadOp  = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
        backupOddColorAttachmentDescription.stencilStoreOp = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        backupOddColorAttachmentDescription.initialLayout  = RHI_IMAGE_LAYOUT_UNDEFINED;
        backupOddColorAttachmentDescription.finalLayout    = RHI_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        RHIAttachmentDescription& backupEvenColorAttachmentDescription =
            attachments[_main_camera_pass_backup_buffer_even];
        backupEvenColorAttachmentDescription.format =
            Framebuffer.Attachments[_main_camera_pass_backup_buffer_even].Format;
        backupEvenColorAttachmentDescription.samples        = RHI_SAMPLE_COUNT_1_BIT;
        backupEvenColorAttachmentDescription.loadOp         = RHI_ATTACHMENT_LOAD_OP_CLEAR;
        backupEvenColorAttachmentDescription.storeOp        = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        backupEvenColorAttachmentDescription.stencilLoadOp  = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
        backupEvenColorAttachmentDescription.stencilStoreOp = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        backupEvenColorAttachmentDescription.initialLayout  = RHI_IMAGE_LAYOUT_UNDEFINED;
        backupEvenColorAttachmentDescription.finalLayout    = RHI_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        RHIAttachmentDescription& depthAttachmentDescription = attachments[_main_camera_pass_depth];
        depthAttachmentDescription.format                   = m_RHI->GetDepthImageInfo().depthImageFormat;
        depthAttachmentDescription.samples                  = RHI_SAMPLE_COUNT_1_BIT;
        depthAttachmentDescription.loadOp                   = RHI_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDescription.storeOp                  = RHI_ATTACHMENT_STORE_OP_STORE;
        depthAttachmentDescription.stencilLoadOp            = RHI_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDescription.stencilStoreOp           = RHI_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.initialLayout            = RHI_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.finalLayout              = RHI_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        RHISubpassDescription subpasses[_main_camera_subpass_count] = {};

        RHIAttachmentReference basePassColorAttachmentsReference[1] = {};
        basePassColorAttachmentsReference[0].attachment = &colorAttachmentDescription - attachments;
        basePassColorAttachmentsReference[0].layout     = RHI_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        RHIAttachmentReference basePassDepthAttachmentReference {};
        basePassDepthAttachmentReference.attachment = &depthAttachmentDescription - attachments;
        basePassDepthAttachmentReference.layout     = RHI_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        RHISubpassDescription& basePass = subpasses[_main_camera_subpass_basepass];
        basePass.pipelineBindPoint     = RHI_PIPELINE_BIND_POINT_GRAPHICS;
        basePass.colorAttachmentCount =
            sizeof(basePassColorAttachmentsReference) / sizeof(basePassColorAttachmentsReference[0]);
        basePass.pColorAttachments       = &basePassColorAttachmentsReference[0];
        basePass.pDepthStencilAttachment = &basePassDepthAttachmentReference;
        basePass.preserveAttachmentCount = 0;
        basePass.pPreserveAttachments    = nullptr;

        RHIAttachmentReference uiPassColorAttachmentReference {};
        uiPassColorAttachmentReference.attachment = &backupEvenColorAttachmentDescription - attachments;
        uiPassColorAttachmentReference.layout     = RHI_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        uint32_t guiPassPreserveAttachment = &backupOddColorAttachmentDescription - attachments;

        RHISubpassDescription& guiPass  = subpasses[_main_camera_subpass_gui];
        guiPass.pipelineBindPoint       = RHI_PIPELINE_BIND_POINT_GRAPHICS;
        guiPass.inputAttachmentCount    = 0;
        guiPass.pInputAttachments       = nullptr;
        guiPass.colorAttachmentCount    = 1;
        guiPass.pColorAttachments       = &uiPassColorAttachmentReference;
        guiPass.pDepthStencilAttachment = nullptr;
        guiPass.preserveAttachmentCount = 1;
        guiPass.pPreserveAttachments    = &guiPassPreserveAttachment;

        RHISubpassDependency dependencies[1] = {};

        RHISubpassDependency& guiPassDependOnBasePass = dependencies[0];
        guiPassDependOnBasePass.srcSubpass           = _main_camera_subpass_basepass;
        guiPassDependOnBasePass.dstSubpass           = _main_camera_subpass_gui;
        guiPassDependOnBasePass.srcStageMask =
            RHI_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | RHI_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        guiPassDependOnBasePass.dstStageMask =
            RHI_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | RHI_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        guiPassDependOnBasePass.srcAccessMask =
            RHI_ACCESS_SHADER_WRITE_BIT | RHI_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        guiPassDependOnBasePass.dstAccessMask =
            RHI_ACCESS_SHADER_READ_BIT | RHI_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        guiPassDependOnBasePass.dependencyFlags = RHI_DEPENDENCY_BY_REGION_BIT;

        RHIRenderPassCreateInfo renderpassCreateInfo {};
        renderpassCreateInfo.sType           = RHI_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassCreateInfo.attachmentCount = (sizeof(attachments) / sizeof(attachments[0]));
        renderpassCreateInfo.pAttachments    = attachments;
        renderpassCreateInfo.subpassCount    = (sizeof(subpasses) / sizeof(subpasses[0]));
        renderpassCreateInfo.pSubpasses      = subpasses;
        renderpassCreateInfo.dependencyCount = (sizeof(dependencies) / sizeof(dependencies[0]));
        renderpassCreateInfo.pDependencies   = dependencies;

        if (m_RHI->CreateRenderPass(&renderpassCreateInfo, Framebuffer.RenderPass) != RHI_SUCCESS)
        {
            throw std::runtime_error("[MainCameraPass] Failed to create render pass!");
        }
    }

    void MainCameraPass::SetupDescriptorSetLayout() {}

    void MainCameraPass::SetupPipelines() {}

    void MainCameraPass::SetupDescriptorSet() {}

    void MainCameraPass::SetupFramebufferDescriptorSet() {}

    void MainCameraPass::SetupSwapchainFramebuffers() {}
} // namespace Galaxy