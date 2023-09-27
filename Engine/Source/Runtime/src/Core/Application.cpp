#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Macro.h"
#include "ImGuiExample.h"

namespace Galaxy
{
    bool Application::Init()
    {
        Logger::Init();
        GAL_CORE_INFO("App Init");
        return ImGuiExample::Init();
    }

    void Application::Run()
    {
        while (m_IsRunning)
        {
            if (!ImGuiExample::Run())
            {
                m_IsRunning = false;
            }
        }
    }

    void Application::Shutdown()
    {
        GAL_CORE_INFO("App Shutdown");
        ImGuiExample::Shutdown();
    }
} // namespace Galaxy