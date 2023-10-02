#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Core/Event/Event.h"
#include "GalaxyEngine/Core/Time/TimeStep.h"

namespace Galaxy
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");

        virtual ~Layer() = default;

        virtual void OnAttach() {}

        virtual void OnDetach() {}

        virtual void OnUpdate(TimeStep ts) {}

        virtual void OnEvent(Event& event) {}

        inline const std::string& GetName() const { return m_DebugName; }

    protected:
        std::string m_DebugName;
    };
} // namespace Galaxy