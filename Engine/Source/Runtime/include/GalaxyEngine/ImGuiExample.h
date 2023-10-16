//
// ImGuiExample.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#pragma once

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class ImGuiExample
{
public:
    static bool Init();
    static bool Run();
    static void Shutdown();

private:
    static GLFWwindow*               s_Window;
    static VkResult                  s_Error;
    static bool                      s_ShowDemoWindow;
    static bool                      s_ShowAnotherWindow;
    static ImVec4                    s_ClearColor;
    static ImGuiIO                   s_IO;
    static ImGui_ImplVulkanH_Window* s_WD;
};