#pragma once

#include "Core/Window.h"
#include "Function/Renderer/RHI/RHIGraphicsContext.h"
#include <GLFW/glfw3.h>

namespace Galaxy
{
    class CrossPlatformWindow : public Window
    {
    public:
        CrossPlatformWindow(const WindowProps& props);

        virtual ~CrossPlatformWindow();

        bool OnUpdate() override;

        void OnRender() override;

        unsigned int GetWidth() const override { return m_Data.Width; }

        unsigned int GetHeight() const override { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

        virtual void* GetNativeWindow() const { return m_Window; }

    private:
        virtual void Init(const WindowProps& props);

        virtual void Shutdown();

    private:
        GLFWwindow*            m_Window;
        Scope<RHIGraphicsContext> m_GraphicsContext;

        struct WindowData
        {
            std::string  Title;
            unsigned int Width, Height;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
} // namespace Galaxy