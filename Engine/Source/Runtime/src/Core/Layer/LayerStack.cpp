//
// LayerStack.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:11.
//

#include "GalaxyEngine/Core/Layer/LayerStack.h"

namespace Galaxy
{
    LayerStack::~LayerStack()
    {
        for (auto layer : m_Layers)
        {
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(Ref<Layer> layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
    }

    void LayerStack::PushOverlay(Ref<Layer> overlay) { m_Layers.emplace_back(overlay); }

    void LayerStack::PopLayer(Ref<Layer> layer)
    {
        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Ref<Layer> overlay)
    {
        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }
} // namespace Galaxy