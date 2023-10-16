//
// VulkanGraphicsContext.cpp
//
// Created or modified by Kexuan Zhang on 2023/10/16 15:24.
//

#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"
#include "GalaxyEngine/Core/Application.h"
#include "GalaxyEngine/Core/Macro.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanMacro.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanShader.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanUtils.h"
#include "GalaxyEngine/Platform/Platform.h"

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
    VulkanGraphicsContext* g_VulkanGraphicsContextPtr = nullptr;

    void VulkanGraphicsContext::Init(void* window)
    {
        GAL_CORE_ASSERT(window, "[VulkanGraphicsContext] GLFW Window handle is null!");
        m_Window = (GLFWwindow*)window;

        g_VulkanGraphicsContextPtr = this;

        CreateInstance();
        SetupDebugCallback();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects();
        GAL_CORE_INFO("[VulkanGraphicsContext] Initiated");
    }

    void VulkanGraphicsContext::Release()
    {
        CleanupSwapChain();

        vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);

        vkDestroyRenderPass(Device, RenderPass, nullptr);

        for (int i = 0; i < MaxFramesInFlight; ++i)
        {
            vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(Device, InFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(Device, CommandPool, nullptr);

        vkDestroyDevice(Device, nullptr);

        if (g_EnableValidationLayers)
        {
            VulkanUtils::DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(Instance, Surface, nullptr);

        vkDestroyInstance(Instance, nullptr);

        GAL_CORE_INFO("[VulkanGraphicsContext] Released");
    }

    void VulkanGraphicsContext::RecreateSwapChain()
    {
        vkDeviceWaitIdle(Device);

        CleanupSwapChain();

        CreateSwapChain();
        CreateImageViews();
        CreateFramebuffers();
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

        auto result = vkCreateInstance(&createInfo, nullptr, &Instance);
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

        auto result = VulkanUtils::CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugMessenger);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to setup debug callback!");
    }

    void VulkanGraphicsContext::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);
        GAL_CORE_ASSERT(deviceCount, "[VulkanGraphicsContext] Failed to find GPU with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (VulkanUtils::IsDeviceSuitable(device, Surface))
            {
                PhysicalDevice = device;
                break;
            }
        }

        GAL_CORE_ASSERT(PhysicalDevice != VK_NULL_HANDLE, "[VulkanGraphicsContext] Failed to find a suitable GPU!");
    }

    void VulkanGraphicsContext::CreateSurface()
    {
        auto result = glfwCreateWindowSurface(Instance, m_Window, nullptr, &Surface);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create window surface!");
    }

    void VulkanGraphicsContext::CreateLogicalDevice()
    {
        auto indices = VulkanUtils::FindQueueFamilies(PhysicalDevice, Surface);

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

        auto result = vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create logical device!");

        vkGetDeviceQueue(Device, indices.GraphicsFamily, 0, &GraphicsQueue);
        vkGetDeviceQueue(Device, indices.PresentFamily, 0, &PresentQueue);
    }

    void VulkanGraphicsContext::CreateSwapChain()
    {
        auto swapChainSupport = VulkanUtils::QuerySwapChainSupport(PhysicalDevice, Surface);

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
        createInfo.surface                  = Surface;
        createInfo.minImageCount            = imageCount;
        createInfo.imageFormat              = surfaceFormat.format;
        createInfo.imageColorSpace          = surfaceFormat.colorSpace;
        createInfo.imageExtent              = extent;
        createInfo.imageArrayLayers         = 1;
        createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto     indices              = VulkanUtils::FindQueueFamilies(PhysicalDevice, Surface);
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

        auto result = vkCreateSwapchainKHR(Device, &createInfo, nullptr, &SwapChain);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create swap chain!");

        vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, nullptr);
        SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(Device, SwapChain, &imageCount, SwapChainImages.data());

        SwapChainImageFormat = surfaceFormat.format;
        SwapChainExtent      = extent;
    }

    void VulkanGraphicsContext::CleanupSwapChain()
    {
        for (size_t i = 0; i < SwapChainFrameBuffers.size(); ++i)
        {
            vkDestroyFramebuffer(Device, SwapChainFrameBuffers[i], nullptr);
        }

        for (size_t i = 0; i < SwapChainImageViews.size(); ++i)
        {
            vkDestroyImageView(Device, SwapChainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(Device, SwapChain, nullptr);
    }

    void VulkanGraphicsContext::CreateImageViews()
    {
        SwapChainImageViews.resize(SwapChainImages.size());

        for (size_t i = 0; i < SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = SwapChainImages[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = SwapChainImageFormat;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            auto result = vkCreateImageView(Device, &createInfo, nullptr, &SwapChainImageViews[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create image views!");
        }
    }

    void VulkanGraphicsContext::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format                  = SwapChainImageFormat;
        colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment            = 0;
        colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount        = 1;
        renderPassInfo.pAttachments           = &colorAttachment;
        renderPassInfo.subpassCount           = 1;
        renderPassInfo.pSubpasses             = &subpass;

        auto result = vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create render pass!");
    }

    void VulkanGraphicsContext::CreateGraphicsPipeline()
    {
        auto vertextShaderModule  = Galaxy::VulkanShader(GAL_RELATIVE_PATH("Resources/Shaders/spv/triangle.vert.spv").string());
        auto fragmentShaderModule = Galaxy::VulkanShader(GAL_RELATIVE_PATH("Resources/Shaders/spv/triangle.frag.spv").string());

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module                          = vertextShaderModule.GetModule();
        vertShaderStageInfo.pName                           = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module                          = fragmentShaderModule.GetModule();
        fragShaderStageInfo.pName                           = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount   = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = (float)SwapChainExtent.width;
        viewport.height     = (float)SwapChainExtent.height;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        VkRect2D scissor = {};
        scissor.offset   = {0, 0};
        scissor.extent   = SwapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount                     = 1;
        viewportState.pViewports                        = &viewport;
        viewportState.scissorCount                      = 1;
        viewportState.pScissors                         = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable                       = VK_FALSE;
        rasterizer.rasterizerDiscardEnable                = VK_FALSE;
        rasterizer.polygonMode                            = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth                              = 1.0f;
        rasterizer.cullMode                               = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace                              = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable                        = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable                  = VK_FALSE;
        multisampling.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable                       = VK_FALSE;
        colorBlending.attachmentCount                     = 1;
        colorBlending.pAttachments                        = &colorBlendAttachment;

        VkDynamicState                   dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState    = {};
        dynamicState.sType                               = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount                   = 2;
        dynamicState.pDynamicStates                      = dynamicStates;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkResult result = vkCreatePipelineLayout(Device, &pipelineLayoutInfo, nullptr, &PipelineLayout);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages    = shaderStages;

        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pDepthStencilState  = nullptr;
        pipelineInfo.pColorBlendState    = &colorBlending;
        pipelineInfo.pDynamicState       = &dynamicState;

        pipelineInfo.layout = PipelineLayout;

        pipelineInfo.renderPass = RenderPass;
        pipelineInfo.subpass    = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex  = -1;

        result = vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create graphics pipeline!");
    }

    void VulkanGraphicsContext::CreateFramebuffers()
    {
        SwapChainFrameBuffers.resize(SwapChainImageViews.size());

        for (size_t i = 0; i < SwapChainImageViews.size(); ++i)
        {
            VkImageView attachments[] = {SwapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass              = RenderPass;
            framebufferInfo.attachmentCount         = 1;
            framebufferInfo.pAttachments            = attachments;
            framebufferInfo.width                   = SwapChainExtent.width;
            framebufferInfo.height                  = SwapChainExtent.height;
            framebufferInfo.layers                  = 1;

            auto result = vkCreateFramebuffer(Device, &framebufferInfo, nullptr, &SwapChainFrameBuffers[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create framebuffer!");
        }
    }

    void VulkanGraphicsContext::CreateCommandPool()
    {
        Galaxy::VulkanUtils::QueueFamilyIndices queueFamilyIndices =
            Galaxy::VulkanUtils::FindQueueFamilies(PhysicalDevice, Surface);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex        = queueFamilyIndices.GraphicsFamily;

        auto result = vkCreateCommandPool(Device, &poolInfo, nullptr, &CommandPool);
        VK_CHECK(result, "[VulkanGraphicsContext] Failed to create command pool!");
    }

    void VulkanGraphicsContext::CreateCommandBuffers()
    {
        CommandBuffers.resize(MaxFramesInFlight);
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool                 = CommandPool;
        allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount          = (uint32_t)CommandBuffers.size();
        auto result                           = vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers.data());
    }

    void VulkanGraphicsContext::CreateSyncObjects()
    {
        ImageAvailableSemaphores.resize(MaxFramesInFlight);
        RenderFinishedSemaphores.resize(MaxFramesInFlight);
        InFlightFences.resize(MaxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT; // set awake

        for (int i = 0; i < MaxFramesInFlight; ++i)
        {
            auto result = vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create image available semaphore!");

            result = vkCreateSemaphore(Device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create render finished semaphore!");

            result = vkCreateFence(Device, &fenceInfo, nullptr, &InFlightFences[i]);
            VK_CHECK(result, "[VulkanGraphicsContext] Failed to create in flight fence!");
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