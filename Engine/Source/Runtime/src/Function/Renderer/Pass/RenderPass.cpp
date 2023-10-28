//
// RenderPass.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/RenderPass.h"

namespace Galaxy
{
    void RenderPass::Initialize(const RenderPassInitInfo* initInfo)
    {
        GlobalRenderRes =
            &(std::static_pointer_cast<RenderResource>(m_RenderResource)->GlobalRenderRes);
    }

    void RenderPass::Draw() {}

    void RenderPass::PostInitialize() {}

    RHIRenderPass* RenderPass::GetRenderPass() const { return Framebuffer.RenderPass; }

    std::vector<RHIImageView*> RenderPass::GetFramebufferImageViews() const
    {
        std::vector<RHIImageView*> imageViews;
        for (auto& attach : Framebuffer.Attachments)
        {
            imageViews.push_back(attach.View);
        }
        return imageViews;
    }

    std::vector<RHIDescriptorSetLayout*> RenderPass::GetDescriptorSetLayouts() const
    {
        std::vector<RHIDescriptorSetLayout*> layouts;
        for (auto& desc : DescriptorInfos)
        {
            layouts.push_back(desc.Layout);
        }
        return layouts;
    }
} // namespace Galaxy