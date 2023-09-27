#include "Core/Application.h"
#include "Platform/Platform.h"

int main(int argc, char** argv)
{
    printOS();

    Galaxy::Application app;
    if (!app.Init())
    {
        return -1;
    }
    app.Run();
    app.Shutdown();

    return 0;
}