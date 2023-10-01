#include "Function/Global/GlobalContext.h"
#include "Core/LoggerSystem.h"
#include "Platform/Common/GLFWWindowSystem.h"
#include "Platform/Common/StandardFileSystem.h"

namespace Galaxy
{
    RuntimeGlobalContext g_RuntimeGlobalContext;

    void RuntimeGlobalContext::StartSystems()
    {
        LoggerSys = CreateRef<LoggerSystem>();

        FileSys = CreateRef<StandardFileSystem>();

        // Currently, we create GLFW window for Vulkan backend
        WindowSys = CreateRef<GLFWWindowSystem>();
    }

    void RuntimeGlobalContext::ShutdownSystems()
    {
        WindowSys->Shutdown();
        WindowSys.reset();

        FileSys.reset();

        LoggerSys.reset();
    }
} // namespace Galaxy
