#include "Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"
#include "Core/Application.h"
#include "Core/Macro.h"
#include "Function/Renderer/RHI/Vulkan/VulkanMacro.h"
#include "Function/Renderer/RHI/Vulkan/VulkanUtils.h"
#include "Platform/Platform.h"

#include <GLFW/glfw3.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL g_FnDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void*                                       pUserData)
{
    GAL_CORE_ERROR("[VulkanUtils] Validation Layer: {0}", pCallbackData->pMessage);

    return VK_FALSE;
}

namespace Galaxy
{
    VulkanGlobalContext VulkanGraphicsContext::s_GlobalContext = {};

    VulkanGraphicsContext::VulkanGraphicsContext(GLFWwindow* window) : m_Window(window)
    {
        GAL_CORE_ASSERT(window, "[VulkanGraphicsContext] GLFW Window handle is null!");
    }

    void VulkanGraphicsContext::Init()
    {
        CreateInstance();
        SetupDebugCallback();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        GAL_CORE_INFO("[VulkanGraphicsContext] Initiated");
    }

    void VulkanGraphicsContext::Release()
    {
        if (g_EnableValidationLayers)
        {
            VulkanUtils::DestroyDebugUtilsMessengerEXT(s_GlobalContext.Instance, s_GlobalContext.DebugMessenger, nullptr);
        }

        vkDestroySwapchainKHR(s_GlobalContext.Device, s_GlobalContext.SwapChain, nullptr);
        vkDestroyDevice(s_GlobalContext.Device, nullptr);
        vkDestroySurfaceKHR(s_GlobalContext.Instance, s_GlobalContext.Surface, nullptr);
        vkDestroyInstance(s_GlobalContext.Instance, nullptr);
    }

    void VulkanGraphicsContext::CreateInstance()
    {
        if (g_EnableValidationLayers)
        {
            GAL_CORE_ASSERT(CheckValidationLayerSupport(),
                            "[VulkanGraphicsContext] Validation layers requested but not available!");
        }

        // 1. Create Application Info
        VkApplicationInfo appInfo  = {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = Application::GetInstance().GetSpecification().Name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName        = nullptr;
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_1;

        // 2. Create Instance
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo     = &appInfo;

        // Flags
#ifdef GAL_PLATFORM_DARWIN
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        // Enable required extensions
        auto requiredExtensions = GetRequiredExtensions();
#ifdef GAL_PLATFORM_DARWIN
        requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        // Enable or disable validation layers
        if (g_EnableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(g_ValidationLayers.size());
            createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        auto result = vkCreateInstance(&createInfo, nullptr, &s_GlobalContext.Instance);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create instance!");

        // 3. Check extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        GAL_CORE_INFO("[VulkanGraphicsContext] Available Vulkan Extensions: ");
        for (const auto& extension : extensions)
        {
            GAL_CORE_INFO("[VulkanGraphicsContext]     {0}", extension.extensionName);
        }
    }

    void VulkanGraphicsContext::SetupDebugCallback()
    {
        if (!g_EnableValidationLayers)
            return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = g_FnDebugCallback;
        createInfo.pUserData       = nullptr; // Optional

        auto result = VulkanUtils::CreateDebugUtilsMessengerEXT(s_GlobalContext.Instance, &createInfo, nullptr, &s_GlobalContext.DebugMessenger);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to setup debug callback!");
    }

    void VulkanGraphicsContext::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(s_GlobalContext.Instance, &deviceCount, nullptr);
        GAL_CORE_ASSERT(deviceCount, "[VulkanGraphicsContext] Failed to find GPU with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(s_GlobalContext.Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (VulkanUtils::IsDeviceSuitable(device, s_GlobalContext.Surface))
            {
                s_GlobalContext.PhysicalDevice = device;
                break;
            }
        }

        GAL_CORE_ASSERT(s_GlobalContext.PhysicalDevice != VK_NULL_HANDLE, "[VulkanGraphicsContext] Failed to find a suitable GPU!");
    }

    void VulkanGraphicsContext::CreateSurface()
    {
        auto result = glfwCreateWindowSurface(s_GlobalContext.Instance, m_Window, nullptr, &s_GlobalContext.Surface);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create window surface!");
    }

    void VulkanGraphicsContext::CreateLogicalDevice()
    {
        auto indices = VulkanUtils::FindQueueFamilies(s_GlobalContext.PhysicalDevice, s_GlobalContext.Surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int>                        uniqueQueueFamilies = {indices.GraphicsFamily, indices.PresentFamily};

        float queuePriority = 1.0f;
        for (const auto& queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex        = queueFamily;
            queueCreateInfo.queueCount              = 1;
            queueCreateInfo.pQueuePriorities        = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};

        // Create logical device
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos    = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount   = static_cast<uint32_t>(g_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = g_DeviceExtensions.data();

        if (g_EnableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(g_ValidationLayers.size());
            createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        auto result = vkCreateDevice(s_GlobalContext.PhysicalDevice, &createInfo, nullptr, &s_GlobalContext.Device);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create logical device!");

        vkGetDeviceQueue(s_GlobalContext.Device, indices.GraphicsFamily, 0, &s_GlobalContext.GraphicsQueue);
        vkGetDeviceQueue(s_GlobalContext.Device, indices.PresentFamily, 0, &s_GlobalContext.PresentQueue);
    }

    void VulkanGraphicsContext::CreateSwapChain()
    {
        auto swapChainSupport = VulkanUtils::QuerySwapChainSupport(s_GlobalContext.PhysicalDevice, s_GlobalContext.Surface);

        auto surfaceFormat = VulkanUtils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        auto presentMode   = VulkanUtils::ChooseSwapPresentMode(swapChainSupport.PresentModes);

        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        auto extent = VulkanUtils::ChooseSwapExtent(swapChainSupport.Capabilities, width, height);

        // Triple buffer
        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

        if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface                  = s_GlobalContext.Surface;
        createInfo.minImageCount            = imageCount;
        createInfo.imageFormat              = surfaceFormat.format;
        createInfo.imageColorSpace          = surfaceFormat.colorSpace;
        createInfo.imageExtent              = extent;
        createInfo.imageArrayLayers         = 1;
        createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto     indices              = VulkanUtils::FindQueueFamilies(s_GlobalContext.PhysicalDevice, s_GlobalContext.Surface);
        uint32_t queueFamilyIndices[] = {(uint32_t)indices.GraphicsFamily, (uint32_t)indices.PresentFamily};

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices   = nullptr;
        }

        createInfo.preTransform   = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = presentMode;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        auto result = vkCreateSwapchainKHR(s_GlobalContext.Device, &createInfo, nullptr, &s_GlobalContext.SwapChain);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create swap chain!");

        vkGetSwapchainImagesKHR(s_GlobalContext.Device, s_GlobalContext.SwapChain, &imageCount, nullptr);
        s_GlobalContext.SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(s_GlobalContext.Device, s_GlobalContext.SwapChain, &imageCount, s_GlobalContext.SwapChainImages.data());

        s_GlobalContext.SwapChainImageFormat = surfaceFormat.format;
        s_GlobalContext.SwapChainExtent      = extent;
    }

    void VulkanGraphicsContext::CreateImageViews()
    {
        s_GlobalContext.SwapChainImageViews.resize(s_GlobalContext.SwapChainImages.size());

        for (size_t i = 0; i < s_GlobalContext.SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = s_GlobalContext.SwapChainImages[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = s_GlobalContext.SwapChainImageFormat;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            auto result = vkCreateImageView(s_GlobalContext.Device, &createInfo, nullptr, &s_GlobalContext.SwapChainImageViews[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create image views!");
        }
    }

    bool VulkanGraphicsContext::CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const auto layerName : g_ValidationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    std::vector<const char*> VulkanGraphicsContext::GetRequiredExtensions()
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (g_EnableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
} // namespace Galaxy