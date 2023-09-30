#pragma once

#include "Function/Renderer/RHI/RHIGraphicsContext.h"

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
        GLFWwindow* m_Window = nullptr;

        VkSurfaceKHR             m_Surface        = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkInstance               m_Instance       = VK_NULL_HANDLE;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_Device         = VK_NULL_HANDLE;

        VkQueue m_PresentQueue  = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        VkSwapchainKHR           m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     m_SwapChainImages;
        VkFormat                 m_SwapChainImageFormat;
        VkExtent2D               m_SwapChainExtent;
        std::vector<VkImageView> m_SwapChainImageViews;
    };
} // namespace Galaxy