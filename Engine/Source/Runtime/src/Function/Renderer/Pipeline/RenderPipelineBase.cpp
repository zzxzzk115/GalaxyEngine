//
// RenderPipelineBase.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pipeline/RenderPipelineBase.h"

namespace Galaxy
{
    void RenderPipelineBase::InitializeUIRenderBackend(GUIBackend* guiBackend)
    {
        m_GUIPass->InitializeUIRenderBackend(guiBackend);
    }

    void RenderPipelineBase::ForwardRender() {}
} // namespace Galaxy