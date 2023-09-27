#include "Core/Application.h"
#include "ImGuiExample.h"

#include <iostream>

bool Application::Init()
{
    std::cout << "App Init" << std::endl;
    return ImGuiExample::Init();
}

void Application::Run()
{
    while (m_IsRunning)
    {
        if (!ImGuiExample::Run())
        {
            m_IsRunning = false;
        }
    }
}

void Application::Shutdown()
{
    std::cout << "App Shutdown" << std::endl;
    ImGuiExample::Shutdown();
}