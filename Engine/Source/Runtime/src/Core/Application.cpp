#include "Core/Application.h"
#include "Core/Macro.h"
#include "Core/WindowSystem.h"
#include "Function/Global/GlobalContext.h"

namespace Galaxy
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& specification) : m_Specification(specification)
    {
        g_RuntimeGlobalContext.StartSystems();

        GAL_CORE_ASSERT(!s_Instance, "[Application] Application already exists!");
        s_Instance = this;

        GAL_CORE_INFO("[Application] Initializing...");

        // Set working directory here
        if (!m_Specification.WorkingDirectory.empty())
        {
            std::filesystem::current_path(m_Specification.WorkingDirectory);
        }

        g_RuntimeGlobalContext.WindowSys->Init(WindowInitInfo(m_Specification.Name));
        g_RuntimeGlobalContext.WindowSys->SetEventCallback(GAL_BIND_EVENT_FN(Application::OnEvent));

        GAL_CORE_INFO("[Application] Initiated");
    }

    Application::~Application() { GAL_CORE_INFO("[Application] Good Bye!"); }

    void Application::Run()
    {
        while (m_IsRunning)
        {
            if (!g_RuntimeGlobalContext.WindowSys->OnUpdate())
            {
                break;
            }

            g_RuntimeGlobalContext.WindowSys->OnRender();
        }
    }

    void Application::Shutdown()
    {
        GAL_CORE_INFO("[Application] Shutting down...");
        m_IsRunning = false;
        g_RuntimeGlobalContext.ShutdownSystems();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(GAL_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(GAL_BIND_EVENT_FN(Application::OnWindowResize));
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_IsRunning = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_IsMinimized = true;
            return false;
        }

        m_IsMinimized = false;

        return false;
    }
} // namespace Galaxy