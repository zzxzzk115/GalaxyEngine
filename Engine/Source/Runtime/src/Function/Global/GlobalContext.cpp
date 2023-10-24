//
// GlobalContext.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Function/Global/GlobalContext.h"
#include "GalaxyEngine/Core/LoggerSystem.h"
#include "GalaxyEngine/Platform/Common/GLFWWindowSystem.h"
#include "GalaxyEngine/Platform/Common/StandardFileSystem.h"
#include "GalaxyEngine/Platform/Common/VulkanRenderSystem.h"

namespace Galaxy
{
    RuntimeGlobalContext g_RuntimeGlobalContext;

    void RuntimeGlobalContext::StartSystems(RuntimeGlobalContextInitInfo initInfo)
    {
        LoggerSys = CreateRef<LoggerSystem>();

        FileSys = CreateRef<StandardFileSystem>();

        // Currently, we create GLFW window for Vulkan backend
        WindowSys = CreateRef<GLFWWindowSystem>();
        WindowInitInfo windowInitInfo = {};
        windowInitInfo.Title = initInfo.AppName;
        WindowSys->Init(windowInitInfo);

        // Currently, we create a Vulkan Renderer for all the platforms
        RenderSys = CreateRef<VulkanRenderSystem>();
        RenderSystemInitInfo renderSystemInitInfo = {};
        renderSystemInitInfo.WindowSys = WindowSys;
        RenderSys->Init(renderSystemInitInfo);
    }

    void RuntimeGlobalContext::ShutdownSystems()
    {
        RenderSys->Release();

        WindowSys->Shutdown();
        WindowSys.reset();

        FileSys.reset();

        LoggerSys.reset();
    }
} // namespace Galaxy
