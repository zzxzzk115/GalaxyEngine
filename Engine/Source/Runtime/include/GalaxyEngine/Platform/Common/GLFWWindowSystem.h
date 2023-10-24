//
// GLFWWindowSystem.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#pragma once

#include "GalaxyEngine/Core/WindowSystem.h"
#include <GLFW/glfw3.h>

namespace Galaxy
{
    class GLFWWindowSystem : public WindowSystem
    {
    public:
        virtual void Init(const WindowInitInfo& props) override;

        virtual void Shutdown() override;

        bool OnUpdate() override;

        void OnRender() override;

        unsigned int GetWidth() const override { return m_Data.Width; }

        unsigned int GetHeight() const override { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

        virtual void* GetNativeWindow() const override { return m_Window; }

    private:
        GLFWwindow*               m_Window;

        struct WindowData
        {
            std::string  Title;
            unsigned int Width, Height;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
} // namespace Galaxy