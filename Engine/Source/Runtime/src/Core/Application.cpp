//
// Application.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:26.
//

#include "GalaxyEngine/Core/Application.h"
#include "GalaxyEngine/Core/FileSystem.h"
#include "GalaxyEngine/Core/Macro.h"
#include "GalaxyEngine/Core/Time/Time.h"
#include "GalaxyEngine/Core/WindowSystem.h"
#include "GalaxyEngine/Function/Global/GlobalContext.h"
#include "GalaxyEngine/Function/Renderer/RenderSystem.h"

namespace Galaxy
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& specification) : m_Specification(specification)
    {
        g_RuntimeGlobalContext.StartSystems();

        GAL_CORE_ASSERT(!s_Instance, "[Application] Application already exists!");
        s_Instance = this;

        GAL_CORE_INFO("[Application] Initializing...");

        // Init Executable Directory
        g_RuntimeGlobalContext.FileSys->InitExecutableDirectory(specification.CommandLineArgs.Args[0]);

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
            float    time     = Time::GetTime();
            TimeStep timeStep = time - m_LastFrameTime;
            m_LastFrameTime   = time;

            if (!g_RuntimeGlobalContext.WindowSys->OnUpdate())
            {
                break;
            }

            if (!m_IsMinimized)
            {
                for (auto layer : m_LayerStack)
                {
                    layer->OnUpdate(timeStep);
                }
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

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::PushLayer(Ref<Layer> layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Ref<Layer> overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_IsRunning = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        auto w = e.GetWidth();
        auto h = e.GetHeight();

        if (w == 0 || h == 0)
        {
            m_IsMinimized = true;
            return false;
        }

        g_RuntimeGlobalContext.RenderSys->SetViewport(0, 0, w, h);

        m_IsMinimized = false;

        return false;
    }
} // namespace Galaxy