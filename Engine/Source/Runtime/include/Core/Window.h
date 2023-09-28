#pragma once

#include "Core/Base.h"
#include "Core/Event/Event.h"

namespace Galaxy
{
    struct WindowProps
    {
        std::string Title;
        uint32_t    Width;
        uint32_t    Height;
        bool        VSync;

        WindowProps(const std::string& title  = "Galaxy Engine",
                    uint32_t           width  = 1600,
                    uint32_t           height = 900,
                    bool               vsync  = true) :
            Title(title),
            Width(width), Height(height), VSync(vsync)
        {}
    };

    // Interface representing a desktop system based Window
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual bool OnUpdate() = 0;

        virtual void OnRender() = 0;

        virtual uint32_t GetWidth() const = 0;

        virtual uint32_t GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void* GetNativeWindow() const = 0;

        static Scope<Window> Create(const WindowProps& props = WindowProps());
    };
} // namespace Galaxy