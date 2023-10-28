//
// RenderPassBase.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/RenderPassBase.h"

namespace Galaxy
{
    void RenderPassBase::PostInitialize() {}

    void RenderPassBase::SetCommonInfo(RenderPassCommonInfo commonInfo)
    {
        m_RHI = commonInfo.RHI;
        m_RenderResource = commonInfo.RenderResource;
    }

    void RenderPassBase::InitializeUIRenderBackend(GUIBackend* guiBackend) {}
} // namespace Galaxy