//
// GLFWWindowSystem.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Platform/Common/GLFWWindowSystem.h"

#include "GalaxyEngine/Core/Event/ApplicationEvent.h"
#include "GalaxyEngine/Core/Event/KeyEvent.h"
#include "GalaxyEngine/Core/Event/MouseEvent.h"

namespace Galaxy
{
    static uint8_t s_glfwWindowCount = 0;

    static void GLFWErrorCallback(int error, const char* description)
    {
        GAL_CORE_ERROR("[Window] GLFW Error ({0}) {1}", error, description);
    }

    void GLFWWindowSystem::Init(const WindowInitInfo& props)
    {
        m_Data.Title  = props.Title;
        m_Data.Width  = props.Width;
        m_Data.Height = props.Height;

        GAL_CORE_INFO(
            "[Window] Creating window, name: {0}, resolution: {1} x {2}", props.Title, props.Width, props.Height);

        if (s_glfwWindowCount == 0)
        {
            int result = glfwInit();
            GAL_CORE_ASSERT(result, "[Window] Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        {
            // Disable creating OpenGL Context
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            // Create window
            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
            ++s_glfwWindowCount;
        }

        // Set icon
        // GLFWimage           images[1];
        // RHIPixelColorFormat format;
        // uint32_t            width, height;
        // auto                buffer = ImageTextureImporter::ImportFromPath(
        //     GAL_RELATIVE_PATH("Resources/Textures/GalaxyLogo.png"), false, format, width, height);
        // images[0].width  = width;
        // images[0].height = height;
        // images[0].pixels = (unsigned char*)buffer.Data;
        // glfwSetWindowIcon(m_Window, 1, images);
        // buffer.Release();

        glfwSetWindowUserPointer(m_Window, &m_Data);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width       = width;
            data.Height      = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            WindowData&      data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, true);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void GLFWWindowSystem::Shutdown()
    {
        glfwDestroyWindow(m_Window);
        --s_glfwWindowCount;

        if (s_glfwWindowCount == 0)
        {
            glfwTerminate();
        }
    }

    bool GLFWWindowSystem::OnUpdate()
    {
        glfwPollEvents();
        return true;
    }

    void GLFWWindowSystem::OnRender() {}
} // namespace Galaxy