#include "ExampleLayer.h"

#include <GalaxyEngine/Core/Macro.h>
#include <GalaxyEngine/Function/Global/GlobalContext.h>
#include <GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGlobalContext.h>
#include <GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanMacro.h>
#include <GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanShader.h>
#include <GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanUtils.h>

static const int MaxFramesInFlight = 2;

ExampleLayer::ExampleLayer() : Galaxy::Layer("ExampleLayer") {}

void ExampleLayer::OnAttach()
{
    GAL_INFO("Playground Example Layer OnAttach...");

    // Vulkan has already initialized here.

    // Triangle Demo without abstraction

    // 1. Setup Shader Stages
    auto vertextShaderModule  = Galaxy::VulkanShader(GAL_RELATIVE_PATH("Resources/Shaders/spv/triangle.vert.spv"));
    auto fragmentShaderModule = Galaxy::VulkanShader(GAL_RELATIVE_PATH("Resources/Shaders/spv/triangle.frag.spv"));

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module                          = vertextShaderModule.GetModule();
    vertShaderStageInfo.pName                           = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module                          = fragmentShaderModule.GetModule();
    fragShaderStageInfo.pName                           = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // 2. Setup Fixed Functions
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount        = 0;
    vertexInputInfo.vertexAttributeDescriptionCount      = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable                 = VK_FALSE;

    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = (float)Galaxy::g_VulkanGlobalContext.SwapChainExtent.width;
    viewport.height     = (float)Galaxy::g_VulkanGlobalContext.SwapChainExtent.height;
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {};
    scissor.offset   = {0, 0};
    scissor.extent   = Galaxy::g_VulkanGlobalContext.SwapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount                     = 1;
    viewportState.pViewports                        = &viewport;
    viewportState.scissorCount                      = 1;
    viewportState.pScissors                         = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable                       = VK_FALSE;
    rasterizer.rasterizerDiscardEnable                = VK_FALSE;
    rasterizer.polygonMode                            = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth                              = 1.0f;
    rasterizer.cullMode                               = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace                              = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable                        = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable                  = VK_FALSE;
    multisampling.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable                       = VK_FALSE;
    colorBlending.attachmentCount                     = 1;
    colorBlending.pAttachments                        = &colorBlendAttachment;

    VkDynamicState                   dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState    = {};
    dynamicState.sType                               = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount                   = 2;
    dynamicState.pDynamicStates                      = dynamicStates;

    // 3. Setup Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkResult result =
        vkCreatePipelineLayout(Galaxy::g_VulkanGlobalContext.Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
    VK_CHECK(result, "Failed to create pipeline layout!");

    // 4. Setup Pass & Subpasses
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format                  = Galaxy::g_VulkanGlobalContext.SwapChainImageFormat;
    colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment            = 0;
    colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount        = 1;
    renderPassInfo.pAttachments           = &colorAttachment;
    renderPassInfo.subpassCount           = 1;
    renderPassInfo.pSubpasses             = &subpass;

    result = vkCreateRenderPass(Galaxy::g_VulkanGlobalContext.Device, &renderPassInfo, nullptr, &m_RenderPass);
    VK_CHECK(result, "Failed to create render pass!");

    // 5. Create Vulkan Graphics Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages    = shaderStages;

    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = nullptr;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;

    pipelineInfo.layout = m_PipelineLayout;

    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass    = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex  = -1;

    result = vkCreateGraphicsPipelines(
        Galaxy::g_VulkanGlobalContext.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
    VK_CHECK(result, "Failed to create graphics pipeline!");

    // 6. Create Framebuffers
    m_SwapChainFrameBuffers.resize(Galaxy::g_VulkanGlobalContext.SwapChainImageViews.size());

    for (size_t i = 0; i < Galaxy::g_VulkanGlobalContext.SwapChainImageViews.size(); ++i)
    {
        VkImageView attachments[] = {Galaxy::g_VulkanGlobalContext.SwapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass              = m_RenderPass;
        framebufferInfo.attachmentCount         = 1;
        framebufferInfo.pAttachments            = attachments;
        framebufferInfo.width                   = Galaxy::g_VulkanGlobalContext.SwapChainExtent.width;
        framebufferInfo.height                  = Galaxy::g_VulkanGlobalContext.SwapChainExtent.height;
        framebufferInfo.layers                  = 1;

        result = vkCreateFramebuffer(
            Galaxy::g_VulkanGlobalContext.Device, &framebufferInfo, nullptr, &m_SwapChainFrameBuffers[i]);
        VK_CHECK(result, "Failed to create framebuffer!");
    }

    // 7. Create Command Pool and Buffers
    Galaxy::VulkanUtils::QueueFamilyIndices queueFamilyIndices = Galaxy::VulkanUtils::FindQueueFamilies(
        Galaxy::g_VulkanGlobalContext.PhysicalDevice, Galaxy::g_VulkanGlobalContext.Surface);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex        = queueFamilyIndices.GraphicsFamily;

    result = vkCreateCommandPool(Galaxy::g_VulkanGlobalContext.Device, &poolInfo, nullptr, &m_CommandPool);
    VK_CHECK(result, "Failed to create command pool!");

    m_CommandBuffers.resize(MaxFramesInFlight);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool                 = m_CommandPool;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount          = (uint32_t)m_CommandBuffers.size();
    result = vkAllocateCommandBuffers(Galaxy::g_VulkanGlobalContext.Device, &allocInfo, m_CommandBuffers.data());

    // 8. Create Sync Objects
    m_ImageAvailableSemaphores.resize(MaxFramesInFlight);
    m_RenderFinishedSemaphores.resize(MaxFramesInFlight);
    m_InFlightFences.resize(MaxFramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT; // set awake

    for (int i = 0; i < MaxFramesInFlight; ++i)
    {
        result = vkCreateSemaphore(
            Galaxy::g_VulkanGlobalContext.Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
        VK_CHECK(result, "Failed to create image available semaphore!");

        result = vkCreateSemaphore(
            Galaxy::g_VulkanGlobalContext.Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
        VK_CHECK(result, "Failed to create render finished semaphore!");

        result = vkCreateFence(Galaxy::g_VulkanGlobalContext.Device, &fenceInfo, nullptr, &m_InFlightFences[i]);
        VK_CHECK(result, "Failed to create in flight fence!");
    }
}

void ExampleLayer::OnDetach()
{
    GAL_INFO("Playground Example Layer OnDetach...");

    vkDeviceWaitIdle(Galaxy::g_VulkanGlobalContext.Device);

    for (int i = 0; i < MaxFramesInFlight; ++i)
    {
        vkDestroySemaphore(Galaxy::g_VulkanGlobalContext.Device, m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(Galaxy::g_VulkanGlobalContext.Device, m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(Galaxy::g_VulkanGlobalContext.Device, m_InFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(Galaxy::g_VulkanGlobalContext.Device, m_CommandPool, nullptr);

    for (auto framebuffer : m_SwapChainFrameBuffers)
    {
        vkDestroyFramebuffer(Galaxy::g_VulkanGlobalContext.Device, framebuffer, nullptr);
    }

    vkDestroyPipeline(Galaxy::g_VulkanGlobalContext.Device, m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(Galaxy::g_VulkanGlobalContext.Device, m_PipelineLayout, nullptr);

    vkDestroyRenderPass(Galaxy::g_VulkanGlobalContext.Device, m_RenderPass, nullptr);
}

void ExampleLayer::OnUpdate(Galaxy::TimeStep ts)
{
    // 1. Render and Present!
    vkWaitForFences(Galaxy::g_VulkanGlobalContext.Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(Galaxy::g_VulkanGlobalContext.Device, 1, &m_InFlightFences[m_CurrentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(Galaxy::g_VulkanGlobalContext.Device,
                          Galaxy::g_VulkanGlobalContext.SwapChain,
                          UINT64_MAX,
                          m_ImageAvailableSemaphores[m_CurrentFrame],
                          VK_NULL_HANDLE,
                          &imageIndex);

    vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore          waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
    VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount         = 1;
    submitInfo.pWaitSemaphores            = waitSemaphores;
    submitInfo.pWaitDstStageMask          = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &m_CommandBuffers[m_CurrentFrame];

    VkSemaphore signalSemaphores[]  = {m_RenderFinishedSemaphores[m_CurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    VkResult result =
        vkQueueSubmit(Galaxy::g_VulkanGlobalContext.GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);
    VK_CHECK(result, "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {Galaxy::g_VulkanGlobalContext.SwapChain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(Galaxy::g_VulkanGlobalContext.PresentQueue, &presentInfo);

    m_CurrentFrame = (m_CurrentFrame + 1) % MaxFramesInFlight;

    // 2. Recreate Swapchain
}

void ExampleLayer::OnEvent(Galaxy::Event& e) {}

void ExampleLayer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VK_CHECK(result, "Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_RenderPass;
    renderPassInfo.framebuffer           = m_SwapChainFrameBuffers[imageIndex];

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = Galaxy::g_VulkanGlobalContext.SwapChainExtent;

    VkClearValue clearColor        = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = static_cast<float>(Galaxy::g_VulkanGlobalContext.SwapChainExtent.width);
    viewport.height     = static_cast<float>(Galaxy::g_VulkanGlobalContext.SwapChainExtent.height);
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset   = {0, 0};
    scissor.extent   = Galaxy::g_VulkanGlobalContext.SwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    VK_CHECK(result, "Failed to end recording command buffer!");
}