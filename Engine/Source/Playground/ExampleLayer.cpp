//
// ExampleLayer.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#include "ExampleLayer.h"

#include <GalaxyEngine/Core/Macro.h>

ExampleLayer::ExampleLayer() : Galaxy::Layer("ExampleLayer") {}

void ExampleLayer::OnAttach()
{
    GAL_INFO("Playground Example Layer OnAttach...");
}

void ExampleLayer::OnDetach()
{
    GAL_INFO("Playground Example Layer OnDetach...");
}

void ExampleLayer::OnUpdate(Galaxy::TimeStep ts) {}

void ExampleLayer::OnEvent(Galaxy::Event& e) {}