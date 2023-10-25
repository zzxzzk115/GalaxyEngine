//
// GUIPass.cpp
//
// Created or modified by Kexuan Zhang on 24/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Pass/GUIPass.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"
#include "GalaxyEngine/Function/GUI/GUIBackend.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Galaxy
{
    void GUIPass::Initialize(const RenderPassInitInfo* initInfo)
    {
        RenderPass::Initialize(nullptr);

        Framebuffer.RenderPass = static_cast<const GUIPassInitInfo*>(initInfo)->RenderPass;
    }

    void GUIPass::InitializeUIRenderBackend(GUIBackend* guiBackend)
    {
        m_GUIBackend = guiBackend;

        ImGui_ImplGlfw_InitForVulkan(std::static_pointer_cast<VulkanRHI>(m_RHI)->Window, true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance                  = std::static_pointer_cast<VulkanRHI>(m_RHI)->Instance;
        initInfo.PhysicalDevice            = std::static_pointer_cast<VulkanRHI>(m_RHI)->PhysicalDevice;
        initInfo.Device                    = std::static_pointer_cast<VulkanRHI>(m_RHI)->Device;
        initInfo.QueueFamily               = m_RHI->GetQueueFamilyIndices().graphicsFamily.value();
        initInfo.Queue                     = ((VulkanQueue*)m_RHI->GetGraphicsQueue())->GetResource();
        initInfo.DescriptorPool            = std::static_pointer_cast<VulkanRHI>(m_RHI)->GlobalVkDescriptorPool;
        initInfo.Subpass                   = _main_camera_subpass_gui;

        // may be different from the real swapchain image count
        // see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
        initInfo.MinImageCount = 3;
        initInfo.ImageCount    = 3;
        ImGui_ImplVulkan_Init(&initInfo, ((VulkanRenderPass*)Framebuffer.RenderPass)->GetResource());

        UploadFonts();
    }

    void GUIPass::UploadFonts()
    {
        RHICommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType                       = RHI_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                       = RHI_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool                 = m_RHI->GetCommandPool();
        allocInfo.commandBufferCount          = 1;

        RHICommandBuffer* commandBuffer = new VulkanCommandBuffer();
        if (RHI_SUCCESS != m_RHI->AllocateCommandBuffers(&allocInfo, commandBuffer))
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        RHICommandBufferBeginInfo beginInfo = {};
        beginInfo.sType                    = RHI_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags                    = RHI_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if (RHI_SUCCESS != m_RHI->BeginCommandBuffer(commandBuffer, &beginInfo))
        {
            throw std::runtime_error("Could not create one-time command buffer!");
        }

        ImGui_ImplVulkan_CreateFontsTexture(((VulkanCommandBuffer*)commandBuffer)->GetResource());

        if (RHI_SUCCESS != m_RHI->EndCommandBuffer(commandBuffer))
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        RHISubmitInfo submitInfo {};
        submitInfo.sType              = RHI_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;

        m_RHI->QueueSubmit(m_RHI->GetGraphicsQueue(), 1, &submitInfo, RHI_NULL_HANDLE);
        m_RHI->QueueWaitIdle(m_RHI->GetGraphicsQueue());

        m_RHI->FreeCommandBuffers(m_RHI->GetCommandPool(), 1, commandBuffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void GUIPass::Draw()
    {
        if (m_GUIBackend)
        {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_GUIBackend->PreRender();

            float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
            m_RHI->PushEvent(m_RHI->GetCurrentCommandBuffer(), "ImGUI", color);

            ImGui::Render();

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<VulkanRHI>(m_RHI)->VkCurrentCommandBuffer);

            m_RHI->PopEvent(m_RHI->GetCurrentCommandBuffer());
        }
    }
} // namespace Galaxy