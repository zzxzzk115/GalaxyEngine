//
// RenderPass.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/RenderPass.h"

namespace Galaxy
{
    void RenderPass::Initialize(const RenderPassInitInfo* initInfo) {}

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