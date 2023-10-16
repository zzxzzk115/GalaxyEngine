//
// main.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#include "PlaygroundApp.h"

#include <GalaxyEngine/Core/Application.h>
#include <GalaxyEngine/Platform/Platform.h>

int main(int argc, char** argv)
{
    PrintPlatform();

    Galaxy::ApplicationCommandLineArgs cmdArgs = {};
    cmdArgs.Count                              = argc;
    cmdArgs.Args                               = argv;

    Galaxy::ApplicationSpecification spec = {};
    spec.Name                             = "Galaxy Development Playground";
    spec.CommandLineArgs                  = cmdArgs;

    PlaygroundApp app(spec);
    app.Run();

    return 0;
}