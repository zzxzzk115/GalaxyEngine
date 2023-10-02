#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHIGraphicsContext.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Galaxy
{
    class VulkanGraphicsContext : public RHIGraphicsContext
    {
    public:
        VulkanGraphicsContext(GLFWwindow* window);

        virtual void Init() override;

        virtual void Release() override;

    private:
        void CreateInstance();

        void SetupDebugCallback();

        void PickPhysicalDevice();

        void CreateSurface();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CreateImageViews();

        bool CheckValidationLayerSupport();

        std::vector<const char*> GetRequiredExtensions();

    private:
        GLFWwindow*                m_Window = nullptr;
    };
} // namespace Galaxy