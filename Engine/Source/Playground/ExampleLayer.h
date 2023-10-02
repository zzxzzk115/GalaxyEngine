#pragma once

#include <GalaxyEngine/Core/Layer/Layer.h>

class ExampleLayer : public Galaxy::Layer
{
public:
    ExampleLayer();

    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;

    virtual void OnDetach() override;

    void OnUpdate(Galaxy::TimeStep ts) override;

    void OnEvent(Galaxy::Event& e) override;
};