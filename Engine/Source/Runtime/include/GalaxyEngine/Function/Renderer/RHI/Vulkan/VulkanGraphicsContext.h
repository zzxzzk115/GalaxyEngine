#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHIGraphicsContext.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Galaxy
{
    class VulkanGraphicsContext : public RHIGraphicsContext
    {
    public:
        virtual void Init(void* window) override;

        virtual void Release() override;

        void RecreateSwapChain();

        static const int MaxFramesInFlight = 2;

    public:
        VkSurfaceKHR             Surface        = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
        VkInstance               Instance       = VK_NULL_HANDLE;

        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         Device         = VK_NULL_HANDLE;

        VkQueue PresentQueue  = VK_NULL_HANDLE;
        VkQueue GraphicsQueue = VK_NULL_HANDLE;

        VkSwapchainKHR           SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     SwapChainImages;
        VkFormat                 SwapChainImageFormat;
        VkExtent2D               SwapChainExtent;
        std::vector<VkImageView> SwapChainImageViews;

        std::vector<VkFramebuffer> SwapChainFrameBuffers;

        VkCommandPool                CommandPool;
        std::vector<VkCommandBuffer> CommandBuffers;

        VkPipelineLayout PipelineLayout;
        VkRenderPass     RenderPass;

        VkPipeline GraphicsPipeline;

        std::vector<VkSemaphore> ImageAvailableSemaphores;
        std::vector<VkSemaphore> RenderFinishedSemaphores;
        std::vector<VkFence>     InFlightFences;

        uint32_t CurrentFrame = 0;
        bool FrameBufferResized = false;

    private:
        void CreateInstance();

        void SetupDebugCallback();

        void PickPhysicalDevice();

        void CreateSurface();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CleanupSwapChain();

        void CreateImageViews();

        void CreateRenderPass();

        void CreateGraphicsPipeline();

        void CreateFramebuffers();

        void CreateCommandPool();

        void CreateCommandBuffers();

        void CreateSyncObjects();

        bool CheckValidationLayerSupport();

        std::vector<const char*> GetRequiredExtensions();

    private:
        GLFWwindow* m_Window = nullptr;
    };

    extern VulkanGraphicsContext* g_VulkanGraphicsContextPtr;
} // namespace Galaxy