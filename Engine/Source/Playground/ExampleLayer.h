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
    VkPipelineLayout           m_PipelineLayout;
    VkRenderPass               m_RenderPass;
    VkPipeline                 m_GraphicsPipeline;
    std::vector<VkFramebuffer> m_SwapChainFrameBuffers;
    VkCommandPool              m_CommandPool;
    VkCommandBuffer            m_CommandBuffer;
    VkSemaphore                m_ImageAvailableSemaphore;
    VkSemaphore                m_RenderFinishedSemaphore;
    VkFence                    m_InFlightFence;
};