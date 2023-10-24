//
// GlobalContext.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:10.
//

#pragma once

#include "GalaxyEngine/Core/Base.h"

namespace Galaxy
{
    class LoggerSystem;
    class FileSystem;
    class WindowSystem;
    class RenderSystem;

    struct RuntimeGlobalContextInitInfo
    {
        std::string AppName = "Galaxy Application";
    };

    struct RuntimeGlobalContext
    {
        Ref<LoggerSystem> LoggerSys;
        Ref<FileSystem>   FileSys;
        Ref<WindowSystem> WindowSys;
        Ref<RenderSystem> RenderSys;

    public:
        void StartSystems(RuntimeGlobalContextInitInfo initInfo);
        void ShutdownSystems();
    };

    extern RuntimeGlobalContext g_RuntimeGlobalContext;
} // namespace Galaxy
