//
// ImGuiBackend.cpp
//
// Created or modified by Kexuan Zhang on 25/10/2023.
//

#include "GalaxyEngine/Function/GUI/ImGuiBackend.h"
#include "GalaxyEngine/Core/WindowSystem.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"
#include "GalaxyEngine/Function/Renderer/RenderSystem.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Galaxy
{
    void ImGuiBackend::Initialize(GUIBackendInitInfo initInfo)
    {
        m_RHI = initInfo.RenderSys->GetRHI();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // TODO: Load .ini config
//        auto iniPath = GAL_RELATIVE_PATH("imgui.ini");
//        if (!std::filesystem::exists(iniPath))
//        {
//            ImGui::LoadIniSettingsFromDisk(iniPath.string().c_str());
//        }

        // TODO: Load custom fonts
//        float fontSize = 20.0f;
//        io.Fonts->AddFontFromFileTTF(GAL_RELATIVE_PATH("Resources/Fonts/OpenSans/OpenSans-Bold.ttf").string().c_str(),
//                                     fontSize);
//        io.FontDefault = io.Fonts->AddFontFromFileTTF(
//            GAL_RELATIVE_PATH("Resources/Fonts/OpenSans/OpenSans-Regular.ttf").string().c_str(), fontSize);
//        float baseFontSize = fontSize;
//        // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
//        float iconFontSize = baseFontSize * 2.0f / 3.0f;
//
//        // merge in icons from Font Awesome
//        static const ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
//        ImFontConfig         iconsConfig;
//        iconsConfig.MergeMode        = true;
//        iconsConfig.PixelSnapH       = true;
//        iconsConfig.GlyphMinAdvanceX = iconFontSize;
//        io.Fonts->AddFontFromFileTTF(KT_RELATIVE_PATH("Resources/Fonts/FontAwesome6/" FONT_ICON_FILE_NAME_FAS).string().c_str(),
//                                     iconFontSize,
//                                     &iconsConfig,
//                                     iconsRanges);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
        // ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            style.PopupRounding = style.TabRounding = 6.0f;
        }

        SetDarkThemeColors();

        // initialize imgui vulkan render backend
        initInfo.RenderSys->InitializeUIRenderBackend(this);
    }

    void ImGuiBackend::SetupRenderingConfig(GUIBackendRenderingConfig config)
    {
        ImGui_ImplGlfw_InitForVulkan(std::static_pointer_cast<VulkanRHI>(m_RHI)->Window, true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance                  = std::static_pointer_cast<VulkanRHI>(m_RHI)->Instance;
        initInfo.PhysicalDevice            = std::static_pointer_cast<VulkanRHI>(m_RHI)->PhysicalDevice;
        initInfo.Device                    = std::static_pointer_cast<VulkanRHI>(m_RHI)->Device;
        initInfo.QueueFamily               = m_RHI->GetQueueFamilyIndices().graphicsFamily.value();
        initInfo.Queue                     = ((VulkanQueue*)m_RHI->GetGraphicsQueue())->GetResource();
        initInfo.DescriptorPool            = std::static_pointer_cast<VulkanRHI>(m_RHI)->GlobalVkDescriptorPool;
        initInfo.Subpass                   = config.GUIPassIndex;

        // may be different from the real swapchain image count
        // see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
        initInfo.MinImageCount = 3;
        initInfo.ImageCount    = 3;
        ImGui_ImplVulkan_Init(&initInfo, ((VulkanRenderPass*)config.GUIRenderPass)->GetResource());

        UploadFonts();
    }

    void ImGuiBackend::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiBackend::PreRender()
    {
        // TODO: Remove, Test only!
        ImGui::ShowDemoWindow();
    }

    void ImGuiBackend::End()
    {
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_RHI->PushEvent(m_RHI->GetCurrentCommandBuffer(), "ImGUI", color);

        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), std::static_pointer_cast<VulkanRHI>(m_RHI)->VkCurrentCommandBuffer);

        m_RHI->PopEvent(m_RHI->GetCurrentCommandBuffer());
    }

    void ImGuiBackend::SetDarkThemeColors()
    {
        // From ImGui Style Editor Exported data.
        ImVec4* colors                         = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.28f, 0.28f, 0.28f, 0.00f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.94f);
        colors[ImGuiCol_Border]                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]               = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_TitleBg]               = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.20f, 0.21f, 1.00f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.30f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_Header]                = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        colors[ImGuiCol_Separator]             = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab]                   = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_TabActive]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_TabUnfocused]          = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    void ImGuiBackend::UploadFonts()
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
} // namespace Galaxy