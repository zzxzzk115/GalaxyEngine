#pragma once

#include "Core/Logger.h"

// Core log macros
#define GAL_CORE_TRACE(...) ::Galaxy::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define GAL_CORE_INFO(...) ::Galaxy::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define GAL_CORE_WARN(...) ::Galaxy::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define GAL_CORE_ERROR(...) ::Galaxy::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define GAL_CORE_CRITICAL(...) ::Galaxy::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define GAL_TRACE(...) ::Galaxy::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define GAL_INFO(...) ::Galaxy::Logger::GetClientLogger()->info(__VA_ARGS__)
#define GAL_WARN(...) ::Galaxy::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define GAL_ERROR(...) ::Galaxy::Logger::GetClientLogger()->error(__VA_ARGS__)
#define GAL_CRITICAL(...) ::Galaxy::Logger::GetClientLogger()->critical(__VA_ARGS__)