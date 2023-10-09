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

private:
    VkPipelineLayout             m_PipelineLayout;
    VkRenderPass                 m_RenderPass;
    VkPipeline                   m_GraphicsPipeline;
    std::vector<VkFramebuffer>   m_SwapChainFrameBuffers;
    VkCommandPool                m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    std::vector<VkSemaphore>     m_ImageAvailableSemaphores;
    std::vector<VkSemaphore>     m_RenderFinishedSemaphores;
    std::vector<VkFence>         m_InFlightFences;

    uint32_t m_CurrentFrame = 0;
};