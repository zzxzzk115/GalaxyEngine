#include "Core/Window.h"
#include "Platform/Common/CrossPlatformWindow.h"

namespace Galaxy
{
    Scope<Window> Window::Create(const WindowProps& props) { return CreateScope<CrossPlatformWindow>(props); }
} // namespace Galaxy