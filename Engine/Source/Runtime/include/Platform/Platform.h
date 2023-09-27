#pragma once

#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
#define OS_MACOS
#elif defined(__linux__)
#define OS_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(__ANDROID__)
#define OS_ANDROID
#elif defined(__APPLE__) && defined(__arm__)
#define OS_IOS
#elif defined(__EMSCRIPTEN__)
#define OS_EMSCRIPTEN
#else
#error "Unsupported Platform"
#endif

static void printOS()
{
#ifdef OS_MACOS
    std::cout << "Running on macOS" << std::endl;
#endif

#ifdef OS_LINUX
    std::cout << "Running on Linux" << std::endl;
#endif

#ifdef OS_WINDOWS
    std::cout << "Running on Windows" << std::endl;
#endif

#ifdef OS_ANDROID
    std::cout << "Running on Android" << std::endl;
#endif

#ifdef OS_IOS
    std::cout << "Running on iOS" << std::endl;
#endif

#ifdef OS_EMSCRIPTEN
    std::cout << "Running on Emscripten (WebAssembly)" << std::endl;
#endif
}