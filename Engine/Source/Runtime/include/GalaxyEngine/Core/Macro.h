#pragma once

#include "GalaxyEngine/Core/FileSystem.h"
#include "GalaxyEngine/Core/LoggerSystem.h"
#include "GalaxyEngine/Function/Global/GlobalContext.h"
#include "GalaxyEngine/Platform/Platform.h"

#ifndef NDEBUG
#if defined(GAL_PLATFORM_WINDOWS)
#define GAL_DEBUGBREAK() __debugbreak()
#elif defined(GAL_PLATFORM_LINUX) || defined(GAL_PLATFORM_DARWIN)
#include <signal.h>
#define GAL_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define GAL_ENABLE_ASSERTS
#else
#define GAL_DEBUGBREAK()
#endif

#define GAL_EXPAND_MACRO(x) x
#define GAL_STRINGIFY_MACRO(x) #x

#define GAL_ARRAYSIZE(_ARR) \
    ((int)(sizeof(_ARR) / sizeof(*(_ARR)))) // Size of a static C-style array. Don't use on pointers!

// Core log macros
#define GAL_CORE_TRACE(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetCoreLogger()->trace(__VA_ARGS__)
#define GAL_CORE_INFO(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetCoreLogger()->info(__VA_ARGS__)
#define GAL_CORE_WARN(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetCoreLogger()->warn(__VA_ARGS__)
#define GAL_CORE_ERROR(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetCoreLogger()->error(__VA_ARGS__)
#define GAL_CORE_CRITICAL(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define GAL_TRACE(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetClientLogger()->trace(__VA_ARGS__)
#define GAL_INFO(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetClientLogger()->info(__VA_ARGS__)
#define GAL_WARN(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetClientLogger()->warn(__VA_ARGS__)
#define GAL_ERROR(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetClientLogger()->error(__VA_ARGS__)
#define GAL_CRITICAL(...) ::Galaxy::g_RuntimeGlobalContext.LoggerSys->GetClientLogger()->critical(__VA_ARGS__)

#define BIT(x) (1 << x)

#define GAL_BIND_EVENT_FN(fn) \
    [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Asserts
#ifdef GAL_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the
// default message
#define GAL_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
    { \
        if (!(check)) \
        { \
            GAL##type##ERROR(msg, __VA_ARGS__); \
            GAL_DEBUGBREAK(); \
        } \
    }
#define GAL_INTERNAL_ASSERT_WITH_MSG(type, check, ...) \
    GAL_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define GAL_INTERNAL_ASSERT_NO_MSG(type, check) \
    GAL_INTERNAL_ASSERT_IMPL(type, \
                             check, \
                             "Assertion '{0}' failed at {1}:{2}", \
                             GAL_STRINGIFY_MACRO(check), \
                             std::filesystem::path(__FILE__).filename().string(), \
                             __LINE__)

#define GAL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define GAL_INTERNAL_ASSERT_GET_MACRO(...) \
    GAL_EXPAND_MACRO( \
        GAL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, GAL_INTERNAL_ASSERT_WITH_MSG, GAL_INTERNAL_ASSERT_NO_MSG))

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define GAL_ASSERT(...) GAL_EXPAND_MACRO(GAL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define GAL_CORE_ASSERT(...) GAL_EXPAND_MACRO(GAL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define GAL_ASSERT(...)
#define GAL_CORE_ASSERT(...)
#endif

#define GAL_RELATIVE_PATH(path) Galaxy::g_RuntimeGlobalContext.FileSys->GetExecutableRelativeDirectory(path)