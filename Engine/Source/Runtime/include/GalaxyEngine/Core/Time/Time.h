#pragma once

#include <GLFW/glfw3.h>

namespace Galaxy
{
    class Time
    {
    public:
        static float GetTime() { return glfwGetTime(); }
    };
} // namespace Galaxy