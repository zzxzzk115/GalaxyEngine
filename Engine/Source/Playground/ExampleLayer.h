//
// ExampleLayer.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#pragma once

#include <GalaxyEngine/Core/Layer/Layer.h>

// TODO: Remove, test only!
#include <vulkan/vulkan.h>

class ExampleLayer : public Galaxy::Layer
{
public:
    ExampleLayer();

    virtual ~ExampleLayer() = default;

    virtual void OnAttach() override;

    virtual void OnDetach() override;

    void OnUpdate(Galaxy::TimeStep ts) override;

    void OnEvent(Galaxy::Event& e) override;

private:
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};