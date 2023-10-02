#include "GalaxyEngine/Core/Application.h"
#include "GalaxyEngine/Platform/Platform.h"

int main(int argc, char** argv)
{
    PrintPlatform();

    Galaxy::ApplicationCommandLineArgs cmdArgs = {};
    cmdArgs.Count = argc;
    cmdArgs.Args = argv;

    Galaxy::ApplicationSpecification spec = {};
    spec.Name = "Galaxy Development Playground";
    spec.CommandLineArgs = cmdArgs;
    
    Galaxy::Application app(spec);
    app.Run();

    return 0;
}