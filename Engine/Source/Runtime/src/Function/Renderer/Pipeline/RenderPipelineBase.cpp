//
// RenderPipelineBase.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pipeline/RenderPipelineBase.h"

namespace Galaxy
{
    void RenderPipelineBase::InitializeUIRenderBackend(Ref<GUIBackend> guiBackend)
    {
        m_GUIPass->InitializeUIRenderBackend(guiBackend);
    }
} // namespace Galaxy