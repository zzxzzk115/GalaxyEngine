//
// Platform.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#pragma once

#include <iostream>

#if defined(__APPLE__) && defined(__MACH__)
#define GAL_PLATFORM_DARWIN
#elif defined(__linux__)
#define GAL_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#define GAL_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
#define GAL_PLATFORM_ANDROID
#elif defined(__APPLE__) && defined(__arm__)
#define GAL_PLATFORM_IOS
#elif defined(__EMSCRIPTEN__)
#define GAL_PLATFORM_EMSCRIPTEN
#else
#error "Unsupported Platform"
#endif

static void PrintPlatform()
{
#ifdef GAL_PLATFORM_DARWIN
    std::cout << "Running on macOS" << std::endl;
#endif

#ifdef GAL_PLATFORM_LINUX
    std::cout << "Running on Linux" << std::endl;
#endif

#ifdef GAL_PLATFORM_WINDOWS
    std::cout << "Running on Windows" << std::endl;
#endif

#ifdef GAL_PLATFORM_ANDROID
    std::cout << "Running on Android" << std::endl;
#endif

#ifdef GAL_PLATFORM_IOS
    std::cout << "Running on iOS" << std::endl;
#endif

#ifdef GAL_PLATFORM_EMSCRIPTEN
    std::cout << "Running on Emscripten (WebAssembly)" << std::endl;
#endif
}