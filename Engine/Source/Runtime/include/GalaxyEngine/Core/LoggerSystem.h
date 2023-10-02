#pragma once

#include "GalaxyEngine/Core/Base.h"

// This ignores all warnings raised inside External headers
// clang-format off
#pragma warning(push, 0)

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#pragma warning(pop)
// clang-format on

namespace Galaxy
{
    class LoggerSystem
    {
    public:
        LoggerSystem();

        Ref<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
        Ref<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

    private:
        Ref<spdlog::logger> m_CoreLogger;
        Ref<spdlog::logger> m_ClientLogger;
    };
} // namespace Galaxy