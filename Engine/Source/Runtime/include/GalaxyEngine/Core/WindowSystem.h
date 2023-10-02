#pragma once

#include "GalaxyEngine/Core/Base.h"
#include "GalaxyEngine/Core/Event/Event.h"

namespace Galaxy
{
    struct WindowInitInfo
    {
        std::string Title;
        uint32_t    Width;
        uint32_t    Height;
        bool        VSync;

        WindowInitInfo(const std::string& title  = "Galaxy Engine",
                       uint32_t           width  = 1600,
                       uint32_t           height = 900,
                       bool               vsync  = true) :
            Title(title),
            Width(width), Height(height), VSync(vsync)
        {}
    };

    // Interface representing a desktop system based WindowSystem
    class WindowSystem
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        WindowSystem()          = default;
        virtual ~WindowSystem() = default;

        virtual void Init(const WindowInitInfo& props) = 0;

        virtual void Shutdown() = 0;

        virtual bool OnUpdate() = 0;

        virtual void OnRender() = 0;

        virtual uint32_t GetWidth() const = 0;

        virtual uint32_t GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        virtual void* GetNativeWindow() const = 0;
    };
} // namespace Galaxy