#include "Core/Application.h"
#include "Platform/Platform.h"

int main(int argc, char** argv)
{
    PrintPlatform();

    Galaxy::ApplicationCommandLineArgs cmdArgs = {};
    cmdArgs.Count = argc;
    cmdArgs.Args = argv;

    Galaxy::ApplicationSpecification spec = {};
    spec.Name = "GalaxyDevelopmentPlayground";
    spec.CommandLineArgs = cmdArgs;
    
    Galaxy::Application app(spec);
    app.Run();

    return 0;
}