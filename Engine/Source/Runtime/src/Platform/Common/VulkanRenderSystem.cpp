//
// VulkanRenderSystem.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Platform/Common/VulkanRenderSystem.h"
#include "GalaxyEngine/Function/GUI/GUIBackend.h"
#include "GalaxyEngine/Function/Renderer/Pipeline/BuiltinRenderPipeline.h"
#include "GalaxyEngine/Function/Renderer/Pipeline/RenderPipelineBase.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"
#include "GalaxyEngine/Function/Renderer/Resource/RenderResource.h"

namespace Galaxy
{
    void VulkanRenderSystem::Init(RenderSystemInitInfo initInfo)
    {
        // 1. Init RHI
        RHIInitInfo rhiInitInfo = {};
        rhiInitInfo.WindowSys = initInfo.WindowSys;
        m_RHI = CreateRef<VulkanRHI>();
        m_RHI->Initialize(rhiInitInfo);

        // 2. Init Render Pipeline
        RenderPipelineInitInfo pipelineInitInfo;

        m_RenderResource = CreateRef<RenderResource>();
        m_RenderResource->UploadGlobalRenderResource(m_RHI);

        pipelineInitInfo.RenderResource = m_RenderResource;

        m_RenderPipeline        = CreateRef<BuiltinRenderPipeline>();
        m_RenderPipeline->m_RHI = m_RHI;
        m_RenderPipeline->Initialize(pipelineInitInfo);
    }

    void VulkanRenderSystem::Update(float deltaTime)
    {
        m_RenderPipeline->ForwardRender();
    }

    void VulkanRenderSystem::Release()
    {
        m_RHI.reset();
    }

    Ref<RHI> VulkanRenderSystem::GetRHI() { return m_RHI; }

    void VulkanRenderSystem::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

    void VulkanRenderSystem::InitializeUIRenderBackend(GUIBackend* guiBackend)
    {
        m_RenderPipeline->InitializeUIRenderBackend(guiBackend);
    }
} // namespace Galaxy
