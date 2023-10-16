#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Core/Event/ApplicationEvent.h"
#include "GalaxyEngine/Core/Layer/LayerStack.h"
#include "GalaxyEngine/Core/Macro.h"

namespace Galaxy
{
    struct ApplicationCommandLineArgs
    {
        int    Count = 0;
        char** Args  = nullptr;

        const char* operator[](int index) const
        {
            GAL_CORE_ASSERT(index < Count, "Arguments index < Count");
            return Args[index];
        }
    };

    struct ApplicationSpecification
    {
        std::string                Name = "Galaxy Application";
        std::string                WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();
        void Run();
        void Shutdown();

        void OnEvent(Event& e);

        void PushLayer(Ref<Layer> layer);
        void PushOverlay(Ref<Layer> overlay);

        static Application& GetInstance() { return *s_Instance; }

        const ApplicationSpecification& GetSpecification() const { return m_Specification; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

    private:
        ApplicationSpecification m_Specification;
        bool                     m_IsRunning     = true;
        bool                     m_IsMinimized   = false;
        float                    m_LastFrameTime = 0.0f;
        LayerStack               m_LayerStack;

    private:
        static Application* s_Instance;
    };
} // namespace Galaxy