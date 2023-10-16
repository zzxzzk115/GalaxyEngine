//
// PlaygroundApp.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#pragma once

#include "ExampleLayer.h"

#include <GalaxyEngine/Core/Application.h>
#include <GalaxyEngine/Core/Macro.h>

class PlaygroundApp : public Galaxy::Application
{
public:
    PlaygroundApp(const Galaxy::ApplicationSpecification& specification) : Galaxy::Application(specification)
    {
        GAL_INFO("Hello, {0}!", specification.Name);
        PushLayer(Galaxy::CreateRef<ExampleLayer>());
    }

    ~PlaygroundApp() {}
};