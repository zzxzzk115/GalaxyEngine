#include "Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"
#include "Core/Application.h"
#include "Core/Macro.h"
#include "Function/Renderer/RHI/Vulkan/VulkanMacro.h"
#include "Function/Renderer/RHI/Vulkan/VulkanUtils.h"
#include "Platform/Platform.h"

#include <GLFW/glfw3.h>

#ifndef NDEBUG
const bool                     g_EnableValidationLayers = false;
const std::vector<const char*> g_ValidationLayers       = {"VK_LAYER_LUNARG_standard_validation"};
#else
const bool                     g_EnableValidationLayers = false;
const std::vector<const char*> g_ValidationLayers       = {};
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL g_FnDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void*                                       pUserData)
{
    GAL_CORE_ERROR("[VulkanGraphicsContext] Validation Layer: {0}", pCallbackData->pMessage);

    return VK_FALSE;
}

namespace Galaxy
{
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
        GAL_CORE_INFO("[VulkanGraphicsContext] Initiated");
    }

    void VulkanGraphicsContext::Release()
    {
        if (g_EnableValidationLayers)
        {
            VulkanUtils::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugCallback, nullptr);
        }

        vkDestroyDevice(m_Device, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
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
            createInfo.enabledLayerCount       = static_cast<uint32_t>(g_ValidationLayers.size());
            createInfo.ppEnabledExtensionNames = g_ValidationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
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

        auto result = VulkanUtils::CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugCallback);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to setup debug callback!");
    }

    void VulkanGraphicsContext::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        GAL_CORE_ASSERT(deviceCount, "[VulkanGraphicsContext] Failed to find GPU with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (VulkanUtils::IsDeviceSuitable(device, m_Surface))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        GAL_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "[VulkanGraphicsContext] Failed to find a suitable GPU!");
    }

    void VulkanGraphicsContext::CreateSurface()
    {
        auto result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create window surface!");
    }

    void VulkanGraphicsContext::CreateLogicalDevice()
    {
        auto indices = VulkanUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);

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
        VkDeviceCreateInfo createInfo    = {};
        createInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos     = queueCreateInfos.data();
        createInfo.queueCreateInfoCount  = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures      = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;

        if (g_EnableValidationLayers)
        {
            createInfo.enabledExtensionCount   = static_cast<uint32_t>(g_ValidationLayers.size());
            createInfo.ppEnabledExtensionNames = g_ValidationLayers.data();
        }
        else
        {
            createInfo.enabledExtensionCount = 0;
        }

        auto result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create logical device!");

        vkGetDeviceQueue(m_Device, indices.GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.PresentFamily, 0, &m_PresentQueue);
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