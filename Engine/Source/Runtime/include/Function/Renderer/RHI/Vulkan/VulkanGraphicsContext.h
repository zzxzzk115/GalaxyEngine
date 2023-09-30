#pragma once

#include "Function/Renderer/RHI/RHIGraphicsContext.h"
#include "Function/Renderer/RHI/Vulkan/VulkanGlobalContext.h"

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

        static const VulkanGlobalContext& GetGlobalContext() { return s_GlobalContext; }

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
        static VulkanGlobalContext s_GlobalContext;
        GLFWwindow*                m_Window = nullptr;
    };
} // namespace Galaxy