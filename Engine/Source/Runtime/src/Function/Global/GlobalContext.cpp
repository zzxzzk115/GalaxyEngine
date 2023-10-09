#include "GalaxyEngine/Function/Global/GlobalContext.h"
#include "GalaxyEngine/Core/LoggerSystem.h"
#include "GalaxyEngine/Platform/Common/GLFWWindowSystem.h"
#include "GalaxyEngine/Platform/Common/StandardFileSystem.h"
#include "GalaxyEngine/Platform/Common/VulkanRenderSystem.h"

namespace Galaxy
{
    RuntimeGlobalContext g_RuntimeGlobalContext;

    void RuntimeGlobalContext::StartSystems()
    {
        LoggerSys = CreateRef<LoggerSystem>();

        FileSys = CreateRef<StandardFileSystem>();

        // Currently, we create GLFW window for Vulkan backend
        WindowSys = CreateRef<GLFWWindowSystem>();

        // Currently, we create a Vulkan Renderer for all the platforms
        RenderSys = CreateRef<VulkanRenderSystem>();
        RenderSys->Init();
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
