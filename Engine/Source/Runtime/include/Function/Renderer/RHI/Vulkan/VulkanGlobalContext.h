#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Galaxy
{
    struct VulkanGlobalContext
    {
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
    };
} // namespace Galaxy