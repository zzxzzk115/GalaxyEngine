#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class LoggerSystem;
    class FileSystem;
    class WindowSystem;
    class RenderSystem;

    struct RuntimeGlobalContext
    {
        Ref<LoggerSystem> LoggerSys;
        Ref<FileSystem>   FileSys;
        Ref<WindowSystem> WindowSys;
        Ref<RenderSystem> RenderSys;

    public:
        void StartSystems();
        void ShutdownSystems();
    };

    extern RuntimeGlobalContext g_RuntimeGlobalContext;
} // namespace Galaxy
