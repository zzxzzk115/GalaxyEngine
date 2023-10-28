//
// MainCameraPass.cpp
//
// Created or modified by Kexuan Zhang on 25/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/MainCameraPass.h"
#include "GalaxyEngine/Function/Renderer/RenderMesh.h"
#include "GalaxyEngine/Function/Renderer/Resource/RenderResource.h"

#include "standard_frag.h"
#include "standard_vert.h"

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
        {
            RHIRenderPassBeginInfo renderpassBeginInfo {};
            renderpassBeginInfo.sType             = RHI_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpassBeginInfo.renderPass        = Framebuffer.RenderPass;
            renderpassBeginInfo.framebuffer       = m_SwapchainFramebuffers[currentSwapchainImageIndex];
            renderpassBeginInfo.renderArea.offset = {0, 0};
            renderpassBeginInfo.renderArea.extent = m_RHI->GetSwapchainInfo().extent;

            RHIClearValue clearValues[_main_camera_pass_total_attachment_count];
            clearValues[_main_camera_pass_backup_buffer_odd].color  = {{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[_main_camera_pass_backup_buffer_even].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[_main_camera_pass_depth].depthStencil       = {1.0f, 0};
            renderpassBeginInfo.clearValueCount                    = (sizeof(clearValues) / sizeof(clearValues[0]));
            renderpassBeginInfo.pClearValues                       = clearValues;

            m_RHI->CmdBeginRenderPassPfn(m_RHI->GetCurrentCommandBuffer(), &renderpassBeginInfo, RHI_SUBPASS_CONTENTS_INLINE);
        }

        m_RHI->CmdNextSubpassPfn(m_RHI->GetCurrentCommandBuffer(), RHI_SUBPASS_CONTENTS_INLINE);

        // TODO: draw other passes
        guiPass.Draw();

        m_RHI->EndCommandBufferPfn(m_RHI->GetCurrentCommandBuffer());
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

    void MainCameraPass::SetupDescriptorSetLayout()
    {
        DescriptorInfos.resize(_layout_type_count);

        {
            RHIDescriptorSetLayoutBinding perMeshLayoutBindings[1];

            RHIDescriptorSetLayoutBinding& perMeshLayoutUniformBufferBinding = perMeshLayoutBindings[0];
            perMeshLayoutUniformBufferBinding.binding                       = 0;
            perMeshLayoutUniformBufferBinding.descriptorType                = RHI_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            perMeshLayoutUniformBufferBinding.descriptorCount               = 1;
            perMeshLayoutUniformBufferBinding.stageFlags                    = RHI_SHADER_STAGE_VERTEX_BIT;
            perMeshLayoutUniformBufferBinding.pImmutableSamplers            = nullptr;

            RHIDescriptorSetLayoutCreateInfo perMeshLayoutCreateInfo {};
            perMeshLayoutCreateInfo.sType        = RHI_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            perMeshLayoutCreateInfo.bindingCount = 1;
            perMeshLayoutCreateInfo.pBindings    = perMeshLayoutBindings;

            if (m_RHI->CreateDescriptorSetLayout(&perMeshLayoutCreateInfo, DescriptorInfos[_per_mesh].Layout) != RHI_SUCCESS)
            {
                throw std::runtime_error("[MainCameraPass] Failed to create per mesh layout!");
            }
        }
    }

    void MainCameraPass::SetupPipelines()
    {
        RenderPipelines.resize(_render_pipeline_type_count);

        // mesh gbuffer
        {
            RHIDescriptorSetLayout*      descriptorSetLayouts[1] = {DescriptorInfos[_per_mesh].Layout};
            RHIPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
            pipelineLayoutCreateInfo.sType          = RHI_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount = 1;
            pipelineLayoutCreateInfo.pSetLayouts    = descriptorSetLayouts;

            if (m_RHI->CreatePipelineLayout(&pipelineLayoutCreateInfo, RenderPipelines[_render_pipeline_type_mesh_gbuffer].Layout) != RHI_SUCCESS)
            {
                throw std::runtime_error("[MainCameraPass] Failed to create mesh gbuffer pipeline layout!");
            }

            RHIShader* vertShaderModule = m_RHI->CreateShaderModule(STANDARD_VERT);
            RHIShader* fragShaderModule = m_RHI->CreateShaderModule(STANDARD_FRAG);

            RHIPipelineShaderStageCreateInfo vertPipelineShaderStageCreateInfo {};
            vertPipelineShaderStageCreateInfo.sType  = RHI_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertPipelineShaderStageCreateInfo.stage  = RHI_SHADER_STAGE_VERTEX_BIT;
            vertPipelineShaderStageCreateInfo.module = vertShaderModule;
            vertPipelineShaderStageCreateInfo.pName  = "main";

            RHIPipelineShaderStageCreateInfo fragPipelineShaderStageCreateInfo {};
            fragPipelineShaderStageCreateInfo.sType  = RHI_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragPipelineShaderStageCreateInfo.stage  = RHI_SHADER_STAGE_FRAGMENT_BIT;
            fragPipelineShaderStageCreateInfo.module = fragShaderModule;
            fragPipelineShaderStageCreateInfo.pName  = "main";

            RHIPipelineShaderStageCreateInfo shaderStages[] = {vertPipelineShaderStageCreateInfo,
                                                                fragPipelineShaderStageCreateInfo};

            auto vertexBindingDescriptions   = MeshVertex::GetBindingDescriptions();
            auto vertexAttributeDescriptions = MeshVertex::GetAttributeDescriptions();
            RHIPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
            vertexInputStateCreateInfo.sType = RHI_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputStateCreateInfo.vertexBindingDescriptionCount   = vertexBindingDescriptions.size();
            vertexInputStateCreateInfo.pVertexBindingDescriptions      = &vertexBindingDescriptions[0];
            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
            vertexInputStateCreateInfo.pVertexAttributeDescriptions    = &vertexAttributeDescriptions[0];

            RHIPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {};
            inputAssemblyCreateInfo.sType    = RHI_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyCreateInfo.topology = RHI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssemblyCreateInfo.primitiveRestartEnable = RHI_FALSE;

            RHIPipelineViewportStateCreateInfo viewportStateCreateInfo {};
            viewportStateCreateInfo.sType         = RHI_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportStateCreateInfo.viewportCount = 1;
            viewportStateCreateInfo.pViewports    = m_RHI->GetSwapchainInfo().viewport;
            viewportStateCreateInfo.scissorCount  = 1;
            viewportStateCreateInfo.pScissors     = m_RHI->GetSwapchainInfo().scissor;

            RHIPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo {};
            rasterizationStateCreateInfo.sType = RHI_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationStateCreateInfo.depthClampEnable        = RHI_FALSE;
            rasterizationStateCreateInfo.rasterizerDiscardEnable = RHI_FALSE;
            rasterizationStateCreateInfo.polygonMode             = RHI_POLYGON_MODE_FILL;
            rasterizationStateCreateInfo.lineWidth               = 1.0f;
            rasterizationStateCreateInfo.cullMode                = RHI_CULL_MODE_BACK_BIT;
            rasterizationStateCreateInfo.frontFace               = RHI_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizationStateCreateInfo.depthBiasEnable         = RHI_FALSE;
            rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
            rasterizationStateCreateInfo.depthBiasClamp          = 0.0f;
            rasterizationStateCreateInfo.depthBiasSlopeFactor    = 0.0f;

            RHIPipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
            multisampleStateCreateInfo.sType = RHI_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampleStateCreateInfo.sampleShadingEnable  = RHI_FALSE;
            multisampleStateCreateInfo.rasterizationSamples = RHI_SAMPLE_COUNT_1_BIT;

            RHIPipelineColorBlendAttachmentState colorBlendAttachments[1] = {};
            colorBlendAttachments[0].colorWriteMask = RHI_COLOR_COMPONENT_R_BIT | RHI_COLOR_COMPONENT_G_BIT |
                                                        RHI_COLOR_COMPONENT_B_BIT | RHI_COLOR_COMPONENT_A_BIT;
            colorBlendAttachments[0].blendEnable         = RHI_FALSE;
            colorBlendAttachments[0].srcColorBlendFactor = RHI_BLEND_FACTOR_ONE;
            colorBlendAttachments[0].dstColorBlendFactor = RHI_BLEND_FACTOR_ZERO;
            colorBlendAttachments[0].colorBlendOp        = RHI_BLEND_OP_ADD;
            colorBlendAttachments[0].srcAlphaBlendFactor = RHI_BLEND_FACTOR_ONE;
            colorBlendAttachments[0].dstAlphaBlendFactor = RHI_BLEND_FACTOR_ZERO;
            colorBlendAttachments[0].alphaBlendOp        = RHI_BLEND_OP_ADD;

            RHIPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
            colorBlendStateCreateInfo.sType         = RHI_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlendStateCreateInfo.logicOpEnable = RHI_FALSE;
            colorBlendStateCreateInfo.logicOp       = RHI_LOGIC_OP_COPY;
            colorBlendStateCreateInfo.attachmentCount =
                sizeof(colorBlendAttachments) / sizeof(colorBlendAttachments[0]);
            colorBlendStateCreateInfo.pAttachments      = &colorBlendAttachments[0];
            colorBlendStateCreateInfo.blendConstants[0] = 0.0f;

            RHIPipelineDepthStencilStateCreateInfo depthStencilCreateInfo {};
            depthStencilCreateInfo.sType            = RHI_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilCreateInfo.depthTestEnable  = RHI_TRUE;
            depthStencilCreateInfo.depthWriteEnable = RHI_TRUE;
            depthStencilCreateInfo.depthCompareOp   = RHI_COMPARE_OP_LESS;
            depthStencilCreateInfo.depthBoundsTestEnable = RHI_FALSE;
            depthStencilCreateInfo.stencilTestEnable     = RHI_FALSE;

            RHIDynamicState                   dynamicStates[] = {RHI_DYNAMIC_STATE_VIEWPORT, RHI_DYNAMIC_STATE_SCISSOR};
            RHIPipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
            dynamicStateCreateInfo.sType             = RHI_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateCreateInfo.dynamicStateCount = 2;
            dynamicStateCreateInfo.pDynamicStates    = dynamicStates;

            RHIGraphicsPipelineCreateInfo pipelineInfo {};
            pipelineInfo.sType               = RHI_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount          = 2;
            pipelineInfo.pStages             = shaderStages;
            pipelineInfo.pVertexInputState   = &vertexInputStateCreateInfo;
            pipelineInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
            pipelineInfo.pViewportState      = &viewportStateCreateInfo;
            pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
            pipelineInfo.pMultisampleState   = &multisampleStateCreateInfo;
            pipelineInfo.pColorBlendState    = &colorBlendStateCreateInfo;
            pipelineInfo.pDepthStencilState  = &depthStencilCreateInfo;
            pipelineInfo.layout              = RenderPipelines[_render_pipeline_type_mesh_gbuffer].Layout;
            pipelineInfo.renderPass          = Framebuffer.RenderPass;
            pipelineInfo.subpass             = _main_camera_subpass_basepass;
            pipelineInfo.basePipelineHandle  = RHI_NULL_HANDLE;
            pipelineInfo.pDynamicState       = &dynamicStateCreateInfo;

            if (RHI_SUCCESS != m_RHI->CreateGraphicsPipelines(RHI_NULL_HANDLE,
                                                              1,
                                                              &pipelineInfo,
                                                              RenderPipelines[_render_pipeline_type_mesh_gbuffer].Pipeline))
            {
                throw std::runtime_error("[MainCameraPass] Failed to create mesh gbuffer graphics pipeline!");
            }

            m_RHI->DestroyShaderModule(vertShaderModule);
            m_RHI->DestroyShaderModule(fragShaderModule);
        }
    }

    void MainCameraPass::SetupDescriptorSet()
    {
        SetupModelDescriptorSet();
    }

    void MainCameraPass::SetupFramebufferDescriptorSet()
    {

    }

    void MainCameraPass::SetupSwapchainFramebuffers()
    {

    }

    void MainCameraPass::SetupModelDescriptorSet()
    {
        // update common model's global descriptor set
        RHIDescriptorSetAllocateInfo meshGlobalDescriptorSetAllocInfo;
        meshGlobalDescriptorSetAllocInfo.sType              = RHI_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        meshGlobalDescriptorSetAllocInfo.pNext              = nullptr;
        meshGlobalDescriptorSetAllocInfo.descriptorPool     = m_RHI->GetDescriptorPool();
        meshGlobalDescriptorSetAllocInfo.descriptorSetCount = 1;
        meshGlobalDescriptorSetAllocInfo.pSetLayouts        = &DescriptorInfos[_per_mesh].Layout;

        if (RHI_SUCCESS != m_RHI->AllocateDescriptorSets(&meshGlobalDescriptorSetAllocInfo, DescriptorInfos[_per_mesh].DescriptorSet))
        {
            throw std::runtime_error("[MainCameraPass] Failed to allocate mesh global descriptor set!");
        }

        RHIDescriptorBufferInfo meshBufferInfo = {};
        meshBufferInfo.offset = 0;
        meshBufferInfo.range  = sizeof(MeshPerFrameStorageBufferObject);
        meshBufferInfo.buffer = GlobalRenderRes->Buffer.GlobalUploadRingBuffer;

        RHIWriteDescriptorSet meshDescriptorWritesInfo[1];

        meshDescriptorWritesInfo[0].sType           = RHI_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        meshDescriptorWritesInfo[0].pNext           = nullptr;
        meshDescriptorWritesInfo[0].dstSet          = DescriptorInfos[_per_mesh].DescriptorSet;
        meshDescriptorWritesInfo[0].dstBinding      = 0;
        meshDescriptorWritesInfo[0].dstArrayElement = 0;
        meshDescriptorWritesInfo[0].descriptorType  = RHI_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        meshDescriptorWritesInfo[0].descriptorCount = 1;
        meshDescriptorWritesInfo[0].pBufferInfo     = &meshBufferInfo;

        m_RHI->UpdateDescriptorSets(sizeof(meshDescriptorWritesInfo) / sizeof(meshDescriptorWritesInfo[0]),
                                    meshDescriptorWritesInfo,
                                    0,
                                    nullptr);
    }
} // namespace Galaxy