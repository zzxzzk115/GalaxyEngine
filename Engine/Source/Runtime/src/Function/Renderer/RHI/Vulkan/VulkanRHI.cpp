//
// VulkanRHI.cpp
//
// Created or modified by Kexuan Zhang on 23/10/2023.
//

#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanUtil.h"
#include "GalaxyEngine/Core/Macro.h"
#include "GalaxyEngine/Platform/Common/GLFWWindowSystem.h"
#include "GalaxyEngine/Platform/Platform.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Galaxy
{
    VulkanRHI::~VulkanRHI()
    {
        // TODO
    }

    void VulkanRHI::Initialize(RHIInitInfo initInfo)
    {
        Window = static_cast<GLFWwindow*>(initInfo.WindowSys->GetNativeWindow());

        uint32_t windowWidth = initInfo.WindowSys->GetWidth();
        uint32_t windowHeight = initInfo.WindowSys->GetHeight();

        Viewport = {0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight), 0.0f, 1.0f};
        Scissor  = {{0, 0}, {windowWidth, windowHeight}};

#ifdef GAL_ENABLE_VULKAN_VALIDATION_LAYERS
        m_EnableValidationLayers = true;
        m_EnableDebugUtilsLabel  = true;
#else
        m_EnableValidationLayers = false;
        m_EnableDebugUtilsLabel  = false;
#endif

#ifdef GAL_PLATFORM_DARWIN
        m_EnablePointLightShadow = false;
#else
        m_EnablePointLightShadow = true;
#endif

        CreateInstance();

        InitializeDebugMessenger();

        CreateWindowSurface();

        InitializePhysicalDevice();

        CreateLogicalDevice();

        CreateCommandPool();

        CreateCommandBuffers();

        CreateDescriptorPool();

        CreateSyncPrimitives();

        CreateSwapchain();

        CreateSwapchainImageViews();

        CreateFramebufferImageAndView();

        CreateAssetAllocator();
    }

    void VulkanRHI::PrepareContext()
    {
        VkCurrentCommandBuffer = VkCommandBuffers[CurrentFrameIndex];
        static_cast<VulkanCommandBuffer*>(CurrentCommandBuffer)->SetResource(VkCurrentCommandBuffer);
    }

    void VulkanRHI::Clear()
    {
        if (m_EnableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(Instance, m_DebugMessenger, nullptr);
        }
    }

    void VulkanRHI::WaitForFences()
    {
        VkResult result =
            _vkWaitForFences(Device, 1, &IsFrameInFlightFences[CurrentFrameIndex], VK_TRUE, UINT64_MAX);

        VK_CHECK(result, "[VulkanRHI] Failed to synchronize!");
    }

    bool VulkanRHI::WaitForFences(uint32_t fenceCount, const RHIFence* const* pFences, RHIBool32 waitAll, uint64_t timeout)
    {
        // fence
        int fenceSize = fenceCount;
        std::vector<VkFence> vkFenceList(fenceSize);
        for (int i = 0; i < fenceSize; ++i)
        {
            const auto& rhiFenceElement = pFences[i];
            auto& vkFenceElement = vkFenceList[i];

            vkFenceElement = ((VulkanFence*)rhiFenceElement)->GetResource();
        };

        VkResult result = vkWaitForFences(Device, fenceCount, vkFenceList.data(), waitAll, timeout);
        VK_CHECK_RETURN_BOOLEAN(result, "aaa")
    }

    void VulkanRHI::GetPhysicalDeviceProperties(RHIPhysicalDeviceProperties* pProperties)
    {
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        vkGetPhysicalDeviceProperties(PhysicalDevice, &vkPhysicalDeviceProperties);

        pProperties->apiVersion = vkPhysicalDeviceProperties.apiVersion;
        pProperties->driverVersion = vkPhysicalDeviceProperties.driverVersion;
        pProperties->vendorID = vkPhysicalDeviceProperties.vendorID;
        pProperties->deviceID = vkPhysicalDeviceProperties.deviceID;
        pProperties->deviceType = static_cast<RHIPhysicalDeviceType>(vkPhysicalDeviceProperties.deviceType);
        for (uint32_t i = 0; i < RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE; i++)
        {
            pProperties->deviceName[i] = vkPhysicalDeviceProperties.deviceName[i];
        }
        for (uint32_t i = 0; i < RHI_UUID_SIZE; i++)
        {
            pProperties->pipelineCacheUUID[i] = vkPhysicalDeviceProperties.pipelineCacheUUID[i];
        }
        pProperties->sparseProperties.residencyStandard2DBlockShape = vkPhysicalDeviceProperties.sparseProperties.residencyStandard2DBlockShape;
        pProperties->sparseProperties.residencyStandard2DMultisampleBlockShape = vkPhysicalDeviceProperties.sparseProperties.residencyStandard2DMultisampleBlockShape;
        pProperties->sparseProperties.residencyStandard3DBlockShape = vkPhysicalDeviceProperties.sparseProperties.residencyStandard3DBlockShape;
        pProperties->sparseProperties.residencyAlignedMipSize = vkPhysicalDeviceProperties.sparseProperties.residencyAlignedMipSize;
        pProperties->sparseProperties.residencyNonResidentStrict = vkPhysicalDeviceProperties.sparseProperties.residencyNonResidentStrict;

        pProperties->limits.maxImageDimension1D = vkPhysicalDeviceProperties.limits.maxImageDimension1D;
        pProperties->limits.maxImageDimension2D = vkPhysicalDeviceProperties.limits.maxImageDimension2D;
        pProperties->limits.maxImageDimension3D = vkPhysicalDeviceProperties.limits.maxImageDimension3D;
        pProperties->limits.maxImageDimensionCube = vkPhysicalDeviceProperties.limits.maxImageDimensionCube;
        pProperties->limits.maxImageArrayLayers = vkPhysicalDeviceProperties.limits.maxImageArrayLayers;
        pProperties->limits.maxTexelBufferElements = vkPhysicalDeviceProperties.limits.maxTexelBufferElements;
        pProperties->limits.maxUniformBufferRange = vkPhysicalDeviceProperties.limits.maxUniformBufferRange;
        pProperties->limits.maxStorageBufferRange = vkPhysicalDeviceProperties.limits.maxStorageBufferRange;
        pProperties->limits.maxPushConstantsSize = vkPhysicalDeviceProperties.limits.maxPushConstantsSize;
        pProperties->limits.maxMemoryAllocationCount = vkPhysicalDeviceProperties.limits.maxMemoryAllocationCount;
        pProperties->limits.maxSamplerAllocationCount = vkPhysicalDeviceProperties.limits.maxSamplerAllocationCount;
        pProperties->limits.bufferImageGranularity = vkPhysicalDeviceProperties.limits.bufferImageGranularity;
        pProperties->limits.sparseAddressSpaceSize = vkPhysicalDeviceProperties.limits.sparseAddressSpaceSize;
        pProperties->limits.maxBoundDescriptorSets = vkPhysicalDeviceProperties.limits.maxBoundDescriptorSets;
        pProperties->limits.maxPerStageDescriptorSamplers = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers;
        pProperties->limits.maxPerStageDescriptorUniformBuffers = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers;
        pProperties->limits.maxPerStageDescriptorStorageBuffers = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers;
        pProperties->limits.maxPerStageDescriptorSampledImages = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages;
        pProperties->limits.maxPerStageDescriptorStorageImages = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages;
        pProperties->limits.maxPerStageDescriptorInputAttachments = vkPhysicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments;
        pProperties->limits.maxPerStageResources = vkPhysicalDeviceProperties.limits.maxPerStageResources;
        pProperties->limits.maxDescriptorSetSamplers = vkPhysicalDeviceProperties.limits.maxDescriptorSetSamplers;
        pProperties->limits.maxDescriptorSetUniformBuffers = vkPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers;
        pProperties->limits.maxDescriptorSetUniformBuffersDynamic = vkPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic;
        pProperties->limits.maxDescriptorSetStorageBuffers = vkPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers;
        pProperties->limits.maxDescriptorSetStorageBuffersDynamic = vkPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic;
        pProperties->limits.maxDescriptorSetSampledImages = vkPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages;
        pProperties->limits.maxDescriptorSetStorageImages = vkPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages;
        pProperties->limits.maxDescriptorSetInputAttachments = vkPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments;
        pProperties->limits.maxVertexInputAttributes = vkPhysicalDeviceProperties.limits.maxVertexInputAttributes;
        pProperties->limits.maxVertexInputBindings = vkPhysicalDeviceProperties.limits.maxVertexInputBindings;
        pProperties->limits.maxVertexInputAttributeOffset = vkPhysicalDeviceProperties.limits.maxVertexInputAttributeOffset;
        pProperties->limits.maxVertexInputBindingStride = vkPhysicalDeviceProperties.limits.maxVertexInputBindingStride;
        pProperties->limits.maxVertexOutputComponents = vkPhysicalDeviceProperties.limits.maxVertexOutputComponents;
        pProperties->limits.maxTessellationGenerationLevel = vkPhysicalDeviceProperties.limits.maxTessellationGenerationLevel;
        pProperties->limits.maxTessellationPatchSize = vkPhysicalDeviceProperties.limits.maxTessellationPatchSize;
        pProperties->limits.maxTessellationControlPerVertexInputComponents = vkPhysicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents;
        pProperties->limits.maxTessellationControlPerVertexOutputComponents = vkPhysicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents;
        pProperties->limits.maxTessellationControlPerPatchOutputComponents = vkPhysicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents;
        pProperties->limits.maxTessellationControlTotalOutputComponents = vkPhysicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents;
        pProperties->limits.maxTessellationEvaluationInputComponents = vkPhysicalDeviceProperties.limits.maxTessellationEvaluationInputComponents;
        pProperties->limits.maxTessellationEvaluationOutputComponents = vkPhysicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents;
        pProperties->limits.maxGeometryShaderInvocations = vkPhysicalDeviceProperties.limits.maxGeometryShaderInvocations;
        pProperties->limits.maxGeometryInputComponents = vkPhysicalDeviceProperties.limits.maxGeometryInputComponents;
        pProperties->limits.maxGeometryOutputComponents = vkPhysicalDeviceProperties.limits.maxGeometryOutputComponents;
        pProperties->limits.maxGeometryOutputVertices = vkPhysicalDeviceProperties.limits.maxGeometryOutputVertices;
        pProperties->limits.maxGeometryTotalOutputComponents = vkPhysicalDeviceProperties.limits.maxGeometryTotalOutputComponents;
        pProperties->limits.maxFragmentInputComponents = vkPhysicalDeviceProperties.limits.maxFragmentInputComponents;
        pProperties->limits.maxFragmentOutputAttachments = vkPhysicalDeviceProperties.limits.maxFragmentOutputAttachments;
        pProperties->limits.maxFragmentDualSrcAttachments = vkPhysicalDeviceProperties.limits.maxFragmentDualSrcAttachments;
        pProperties->limits.maxFragmentCombinedOutputResources = vkPhysicalDeviceProperties.limits.maxFragmentCombinedOutputResources;
        pProperties->limits.maxComputeSharedMemorySize = vkPhysicalDeviceProperties.limits.maxComputeSharedMemorySize;
        for (uint32_t i = 0; i < 3; i++)
        {
            pProperties->limits.maxComputeWorkGroupCount[i] = vkPhysicalDeviceProperties.limits.maxComputeWorkGroupCount[i];
        }
        pProperties->limits.maxComputeWorkGroupInvocations = vkPhysicalDeviceProperties.limits.maxComputeWorkGroupInvocations;
        for (uint32_t i = 0; i < 3; i++)
        {
            pProperties->limits.maxComputeWorkGroupSize[i] = vkPhysicalDeviceProperties.limits.maxComputeWorkGroupSize[i];
        }
        pProperties->limits.subPixelPrecisionBits = vkPhysicalDeviceProperties.limits.subPixelPrecisionBits;
        pProperties->limits.subTexelPrecisionBits = vkPhysicalDeviceProperties.limits.subTexelPrecisionBits;
        pProperties->limits.mipmapPrecisionBits = vkPhysicalDeviceProperties.limits.mipmapPrecisionBits;
        pProperties->limits.maxDrawIndexedIndexValue = vkPhysicalDeviceProperties.limits.maxDrawIndexedIndexValue;
        pProperties->limits.maxDrawIndirectCount = vkPhysicalDeviceProperties.limits.maxDrawIndirectCount;
        pProperties->limits.maxSamplerLodBias = vkPhysicalDeviceProperties.limits.maxSamplerLodBias;
        pProperties->limits.maxSamplerAnisotropy = vkPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
        pProperties->limits.maxViewports = vkPhysicalDeviceProperties.limits.maxViewports;
        for (uint32_t i = 0; i < 2; i++)
        {
            pProperties->limits.maxViewportDimensions[i] = vkPhysicalDeviceProperties.limits.maxViewportDimensions[i];
        }
        for (uint32_t i = 0; i < 2; i++)
        {
            pProperties->limits.viewportBoundsRange[i] = vkPhysicalDeviceProperties.limits.viewportBoundsRange[i];
        }
        pProperties->limits.viewportSubPixelBits = vkPhysicalDeviceProperties.limits.viewportSubPixelBits;
        pProperties->limits.minMemoryMapAlignment = vkPhysicalDeviceProperties.limits.minMemoryMapAlignment;
        pProperties->limits.minTexelBufferOffsetAlignment = vkPhysicalDeviceProperties.limits.minTexelBufferOffsetAlignment;
        pProperties->limits.minUniformBufferOffsetAlignment = vkPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
        pProperties->limits.minStorageBufferOffsetAlignment = vkPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
        pProperties->limits.minTexelOffset = vkPhysicalDeviceProperties.limits.minTexelOffset;
        pProperties->limits.maxTexelOffset = vkPhysicalDeviceProperties.limits.maxTexelOffset;
        pProperties->limits.minTexelGatherOffset = vkPhysicalDeviceProperties.limits.minTexelGatherOffset;
        pProperties->limits.maxTexelGatherOffset = vkPhysicalDeviceProperties.limits.maxTexelGatherOffset;
        pProperties->limits.minInterpolationOffset = vkPhysicalDeviceProperties.limits.minInterpolationOffset;
        pProperties->limits.maxInterpolationOffset = vkPhysicalDeviceProperties.limits.maxInterpolationOffset;
        pProperties->limits.subPixelInterpolationOffsetBits = vkPhysicalDeviceProperties.limits.subPixelInterpolationOffsetBits;
        pProperties->limits.maxFramebufferWidth = vkPhysicalDeviceProperties.limits.maxFramebufferWidth;
        pProperties->limits.maxFramebufferHeight = vkPhysicalDeviceProperties.limits.maxFramebufferHeight;
        pProperties->limits.maxFramebufferLayers = vkPhysicalDeviceProperties.limits.maxFramebufferLayers;
        pProperties->limits.framebufferColorSampleCounts = vkPhysicalDeviceProperties.limits.framebufferColorSampleCounts;
        pProperties->limits.framebufferDepthSampleCounts = vkPhysicalDeviceProperties.limits.framebufferDepthSampleCounts;
        pProperties->limits.framebufferStencilSampleCounts = vkPhysicalDeviceProperties.limits.framebufferStencilSampleCounts;
        pProperties->limits.framebufferNoAttachmentsSampleCounts = vkPhysicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts;
        pProperties->limits.maxColorAttachments = vkPhysicalDeviceProperties.limits.maxColorAttachments;
        pProperties->limits.sampledImageColorSampleCounts = vkPhysicalDeviceProperties.limits.sampledImageColorSampleCounts;
        pProperties->limits.sampledImageIntegerSampleCounts = vkPhysicalDeviceProperties.limits.sampledImageIntegerSampleCounts;
        pProperties->limits.sampledImageDepthSampleCounts = vkPhysicalDeviceProperties.limits.sampledImageDepthSampleCounts;
        pProperties->limits.sampledImageStencilSampleCounts = vkPhysicalDeviceProperties.limits.sampledImageStencilSampleCounts;
        pProperties->limits.storageImageSampleCounts = vkPhysicalDeviceProperties.limits.storageImageSampleCounts;
        pProperties->limits.maxSampleMaskWords = vkPhysicalDeviceProperties.limits.maxSampleMaskWords;
        pProperties->limits.timestampComputeAndGraphics = vkPhysicalDeviceProperties.limits.timestampComputeAndGraphics;
        pProperties->limits.timestampPeriod = vkPhysicalDeviceProperties.limits.timestampPeriod;
        pProperties->limits.maxClipDistances = vkPhysicalDeviceProperties.limits.maxClipDistances;
        pProperties->limits.maxCullDistances = vkPhysicalDeviceProperties.limits.maxCullDistances;
        pProperties->limits.maxCombinedClipAndCullDistances = vkPhysicalDeviceProperties.limits.maxCombinedClipAndCullDistances;
        pProperties->limits.discreteQueuePriorities = vkPhysicalDeviceProperties.limits.discreteQueuePriorities;
        for (uint32_t i = 0; i < 2; i++)
        {
            pProperties->limits.pointSizeRange[i] = vkPhysicalDeviceProperties.limits.pointSizeRange[i];
        }
        for (uint32_t i = 0; i < 2; i++)
        {
            pProperties->limits.lineWidthRange[i] = vkPhysicalDeviceProperties.limits.lineWidthRange[i];
        }
        pProperties->limits.pointSizeGranularity = vkPhysicalDeviceProperties.limits.pointSizeGranularity;
        pProperties->limits.lineWidthGranularity = vkPhysicalDeviceProperties.limits.lineWidthGranularity;
        pProperties->limits.strictLines = vkPhysicalDeviceProperties.limits.strictLines;
        pProperties->limits.standardSampleLocations = vkPhysicalDeviceProperties.limits.standardSampleLocations;
        pProperties->limits.optimalBufferCopyOffsetAlignment = vkPhysicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment;
        pProperties->limits.optimalBufferCopyRowPitchAlignment = vkPhysicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment;
        pProperties->limits.nonCoherentAtomSize = vkPhysicalDeviceProperties.limits.nonCoherentAtomSize;
    }

    void VulkanRHI::ResetCommandPool()
    {
        VkResult result = _vkResetCommandPool(Device, CommandPools[CurrentFrameIndex], 0);
        VK_CHECK(result, "[VulkanRHI] Failed to synchronize!");
    }

    bool VulkanRHI::PrepareBeforePass(std::function<void()> passUpdateAfterRecreateSwapchain)
    {
        VkResult result =
            vkAcquireNextImageKHR(Device,
                                  Swapchain,
                                  UINT64_MAX,
                                  ImageAvailableForRenderSemaphores[CurrentFrameIndex],
                                  VK_NULL_HANDLE,
                                  &CurrentSwapchainImageIndex);

        if (VK_ERROR_OUT_OF_DATE_KHR == result)
        {
            RecreateSwapchain();
            passUpdateAfterRecreateSwapchain();
            return RHI_SUCCESS;
        }
        else if (VK_SUBOPTIMAL_KHR == result)
        {
            RecreateSwapchain();
            passUpdateAfterRecreateSwapchain();

            // nullptr submit to wait semaphore
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT};
            VkSubmitInfo         submitInfo   = {};
            submitInfo.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount     = 1;
            submitInfo.pWaitSemaphores        = &ImageAvailableForRenderSemaphores[CurrentFrameIndex];
            submitInfo.pWaitDstStageMask      = waitStages;
            submitInfo.commandBufferCount     = 0;
            submitInfo.pCommandBuffers        = VK_NULL_HANDLE;
            submitInfo.signalSemaphoreCount   = 0;
            submitInfo.pSignalSemaphores      = VK_NULL_HANDLE;

            result = _vkResetFences(Device, 1, &IsFrameInFlightFences[CurrentFrameIndex]);
            VK_CHECK(result, "[VulkanRHI] Failed to reset fences!");

            result = vkQueueSubmit(((VulkanQueue*)GraphicsQueue)->GetResource(), 1, &submitInfo, IsFrameInFlightFences[CurrentFrameIndex]);
            if (VK_SUCCESS != result)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to submit queue!");
                return false;
            }
            CurrentFrameIndex = (CurrentFrameIndex + 1) % MaxFramesInFlight;
            return RHI_SUCCESS;
        }
        else
        {
            if (VK_SUCCESS != result)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to acquire image!");
                return false;
            }
        }

        // begin command buffer
        VkCommandBufferBeginInfo commandBufferBeginInfo {};
        commandBufferBeginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags            = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        VkResult resBeginCommandBuffer =
            _vkBeginCommandBuffer(VkCommandBuffers[CurrentFrameIndex], &commandBufferBeginInfo);

        if (VK_SUCCESS != resBeginCommandBuffer)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to begin command buffer!");
            return false;
        }
        return false;
    }

    void VulkanRHI::SubmitRendering(std::function<void()> passUpdateAfterRecreateSwapchain)
    {
        // end command buffer
        VkResult result = _vkEndCommandBuffer(VkCommandBuffers[CurrentFrameIndex]);
        if (VK_SUCCESS != result)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to end command buffer!");
            return;
        }

        VkSemaphore semaphores[2] = { ((VulkanSemaphore*)ImageAvailableForTexturescopySemaphores[CurrentFrameIndex])->GetResource(),
                                     ImageFinishedForPresentationSemaphores[CurrentFrameIndex] };

        // submit command buffer
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo         submitInfo   = {};
        submitInfo.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount     = 1;
        submitInfo.pWaitSemaphores        = &ImageAvailableForRenderSemaphores[CurrentFrameIndex];
        submitInfo.pWaitDstStageMask      = waitStages;
        submitInfo.commandBufferCount     = 1;
        submitInfo.pCommandBuffers        = &VkCommandBuffers[CurrentFrameIndex];
        submitInfo.signalSemaphoreCount = 2;
        submitInfo.pSignalSemaphores = semaphores;

        result = _vkResetFences(Device, 1, &IsFrameInFlightFences[CurrentFrameIndex]);

        if (VK_SUCCESS != result)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to reset fences!");
            return;
        }
        result = vkQueueSubmit(((VulkanQueue*)GraphicsQueue)->GetResource(), 1, &submitInfo, IsFrameInFlightFences[CurrentFrameIndex]);

        if (VK_SUCCESS != result)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to submit queue!");
            return;
        }

        // present swapchain
        VkPresentInfoKHR presentInfo   = {};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = &ImageFinishedForPresentationSemaphores[CurrentFrameIndex];
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &Swapchain;
        presentInfo.pImageIndices      = &CurrentSwapchainImageIndex;

        result = vkQueuePresentKHR(PresentQueue, &presentInfo);
        if (VK_ERROR_OUT_OF_DATE_KHR == result || VK_SUBOPTIMAL_KHR == result)
        {
            RecreateSwapchain();
            passUpdateAfterRecreateSwapchain();
        }
        else
        {
            if (VK_SUCCESS != result)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to present queue!");
                return;
            }
        }

        CurrentFrameIndex = (CurrentFrameIndex + 1) % MaxFramesInFlight;
    }

    RHICommandBuffer* VulkanRHI::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = ((VulkanCommandPool*)RhiCommandPool)->GetResource();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(Device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        _vkBeginCommandBuffer(commandBuffer, &beginInfo);

        RHICommandBuffer* rhiCommandBuffer = new VulkanCommandBuffer();
        ((VulkanCommandBuffer*)rhiCommandBuffer)->SetResource(commandBuffer);
        return rhiCommandBuffer;
    }

    void VulkanRHI::EndSingleTimeCommands(RHICommandBuffer* commandBuffer)
    {
        VkCommandBuffer vkCommandBuffer = ((VulkanCommandBuffer*)commandBuffer)->GetResource();
        _vkEndCommandBuffer(vkCommandBuffer);

        VkSubmitInfo submitInfo {};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &vkCommandBuffer;

        vkQueueSubmit(((VulkanQueue*)GraphicsQueue)->GetResource(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(((VulkanQueue*)GraphicsQueue)->GetResource());

        vkFreeCommandBuffers(Device, ((VulkanCommandPool*)RhiCommandPool)->GetResource(), 1, &vkCommandBuffer);
        delete(commandBuffer);
    }

    // validation layers
    bool VulkanRHI::CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_ValidationLayers)
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

        return RHI_SUCCESS;
    }

    std::vector<const char*> VulkanRHI::GetRequiredExtensions()
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (m_EnableValidationLayers || m_EnableDebugUtilsLabel)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    // debug callback
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                        VkDebugUtilsMessageTypeFlagsEXT,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void*)
    {
        GAL_CORE_ERROR("[VulkanRHI] Validation layer: {0}", pCallbackData->pMessage);
        return VK_FALSE;
    }

    void VulkanRHI::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo       = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void VulkanRHI::CreateInstance()
    {
        // validation layer will be enabled in debug mode
        if (m_EnableValidationLayers && !CheckValidationLayerSupport())
        {
            GAL_CORE_ERROR("[VulkanRHI] Validation layers requested, but not available!");
        }

        m_VulkanApiVersion = VK_API_VERSION_1_1;

        // app info
        VkApplicationInfo appInfo {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "GalaxyVulkanRenderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Galaxy";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = m_VulkanApiVersion;

        // create info
        VkInstanceCreateInfo instanceCreateInfo {};
        instanceCreateInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo; // the appInfo is stored here

        // Flags
#ifdef GAL_PLATFORM_DARWIN
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        auto extensions          = GetRequiredExtensions();
#ifdef GAL_PLATFORM_DARWIN
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
        instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_EnableValidationLayers)
        {
            instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(m_ValidationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext             = nullptr;
        }

        // create Vulkan Instance
        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);
        VK_CHECK(result, "[VulkanRHI] Failed to create instance!");
    }

    void VulkanRHI::InitializeDebugMessenger()
    {
        if (m_EnableValidationLayers)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            PopulateDebugMessengerCreateInfo(createInfo);

            VkResult result = CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &m_DebugMessenger);
            VK_CHECK(result, "[VulkanRHI] Failed to set up debug messenger!");
        }

        if (m_EnableDebugUtilsLabel)
        {
            _vkCmdBeginDebugUtilsLabelEXT =
                (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(Instance, "vkCmdBeginDebugUtilsLabelEXT");
            _vkCmdEndDebugUtilsLabelEXT =
                (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(Instance, "vkCmdEndDebugUtilsLabelEXT");
        }
    }

    void VulkanRHI::CreateWindowSurface()
    {
        VkResult result = glfwCreateWindowSurface(Instance, Window, nullptr, &Surface);
        VK_CHECK(result, "[VulkanRHI] Failed to create GLFW window surface!");
    }

    void VulkanRHI::InitializePhysicalDevice()
    {
        uint32_t physicalDeviceCount;
        vkEnumeratePhysicalDevices(Instance, &physicalDeviceCount, nullptr);
        if (physicalDeviceCount == 0)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to enumerate physical devices!");
        }
        else
        {
            // find one device that matches our requirement
            // or find which is the best
            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(Instance, &physicalDeviceCount, physicalDevices.data());

            std::vector<std::pair<int, VkPhysicalDevice>> rankedPhysicalDevices;
            for (const auto& device : physicalDevices)
            {
                VkPhysicalDeviceProperties physicalDeviceProperties;
                vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
                int score = 0;

                if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    score += 1000;
                }
                else if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                {
                    score += 100;
                }

                rankedPhysicalDevices.push_back({score, device});
            }

            std::sort(rankedPhysicalDevices.begin(),
                      rankedPhysicalDevices.end(),
                      [](const std::pair<int, VkPhysicalDevice>& p1, const std::pair<int, VkPhysicalDevice>& p2) {
                          return p1 > p2;
                      });

            for (const auto& device : rankedPhysicalDevices)
            {
                if (IsDeviceSuitable(device.second))
                {
                    PhysicalDevice = device.second;
                    break;
                }
            }

            if (PhysicalDevice == VK_NULL_HANDLE)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to find suitable physical device");
            }
        }
    }

    // logical device (m_vulkan_context._device : graphic queue, present queue,
    // feature:samplerAnisotropy)
    void VulkanRHI::CreateLogicalDevice()
    {
        QueueIndices = FindQueueFamilies(PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; // all queues that need to be created
        std::set<uint32_t>                   queueFamilies = {QueueIndices.graphicsFamily.value(),
                                                              QueueIndices.presentFamily.value(),
                                                              QueueIndices.computeFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : queueFamilies) // for every queue family
        {
            // queue create info
            VkDeviceQueueCreateInfo queueCreateInfo {};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount       = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // physical device features
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

        physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

        // support inefficient readback storage buffer
        physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;

        // support independent blending
        physicalDeviceFeatures.independentBlend = VK_TRUE;

        // support geometry shader
        if (m_EnablePointLightShadow)
        {
            physicalDeviceFeatures.geometryShader = VK_TRUE;
        }

        // device create info
        VkDeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos       = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures        = &physicalDeviceFeatures;
        deviceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(m_DeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
        deviceCreateInfo.enabledLayerCount       = 0;

        VkResult result = vkCreateDevice(PhysicalDevice, &deviceCreateInfo, nullptr, &Device);
        VK_CHECK(result, "[VulkanRHI] Failed to create device!");

        // initialize queues of this device
        VkQueue vkGraphicsQueue;
        vkGetDeviceQueue(Device, QueueIndices.graphicsFamily.value(), 0, &vkGraphicsQueue);
        GraphicsQueue = new VulkanQueue();
        ((VulkanQueue*)GraphicsQueue)->SetResource(vkGraphicsQueue);

        vkGetDeviceQueue(Device, QueueIndices.presentFamily.value(), 0, &PresentQueue);

        VkQueue vkComputeQueue;
        vkGetDeviceQueue(Device, QueueIndices.computeFamily.value(), 0, &vkComputeQueue);
        ComputeQueue = new VulkanQueue();
        ((VulkanQueue*)ComputeQueue)->SetResource(vkComputeQueue);

        // more efficient pointer
        _vkResetCommandPool      = (PFN_vkResetCommandPool)vkGetDeviceProcAddr(Device, "vkResetCommandPool");
        _vkBeginCommandBuffer    = (PFN_vkBeginCommandBuffer)vkGetDeviceProcAddr(Device, "vkBeginCommandBuffer");
        _vkEndCommandBuffer      = (PFN_vkEndCommandBuffer)vkGetDeviceProcAddr(Device, "vkEndCommandBuffer");
        _vkCmdBeginRenderPass    = (PFN_vkCmdBeginRenderPass)vkGetDeviceProcAddr(Device, "vkCmdBeginRenderPass");
        _vkCmdNextSubpass        = (PFN_vkCmdNextSubpass)vkGetDeviceProcAddr(Device, "vkCmdNextSubpass");
        _vkCmdEndRenderPass      = (PFN_vkCmdEndRenderPass)vkGetDeviceProcAddr(Device, "vkCmdEndRenderPass");
        _vkCmdBindPipeline       = (PFN_vkCmdBindPipeline)vkGetDeviceProcAddr(Device, "vkCmdBindPipeline");
        _vkCmdSetViewport        = (PFN_vkCmdSetViewport)vkGetDeviceProcAddr(Device, "vkCmdSetViewport");
        _vkCmdSetScissor         = (PFN_vkCmdSetScissor)vkGetDeviceProcAddr(Device, "vkCmdSetScissor");
        _vkWaitForFences         = (PFN_vkWaitForFences)vkGetDeviceProcAddr(Device, "vkWaitForFences");
        _vkResetFences           = (PFN_vkResetFences)vkGetDeviceProcAddr(Device, "vkResetFences");
        _vkCmdDrawIndexed        = (PFN_vkCmdDrawIndexed)vkGetDeviceProcAddr(Device, "vkCmdDrawIndexed");
        _vkCmdBindVertexBuffers  = (PFN_vkCmdBindVertexBuffers)vkGetDeviceProcAddr(Device, "vkCmdBindVertexBuffers");
        _vkCmdBindIndexBuffer    = (PFN_vkCmdBindIndexBuffer)vkGetDeviceProcAddr(Device, "vkCmdBindIndexBuffer");
        _vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)vkGetDeviceProcAddr(Device, "vkCmdBindDescriptorSets");
        _vkCmdClearAttachments   = (PFN_vkCmdClearAttachments)vkGetDeviceProcAddr(Device, "vkCmdClearAttachments");

        DepthImageFormat = (RHIFormat)FindDepthFormat();
    }

    void VulkanRHI::CreateCommandPool()
    {
        // default graphics command pool
        {
            RhiCommandPool = new VulkanCommandPool();
            VkCommandPool vkCommandPool;
            VkCommandPoolCreateInfo commandPoolCreateInfo {};
            commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.pNext            = VK_NULL_HANDLE;
            commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = QueueIndices.graphicsFamily.value();

            VkResult result = vkCreateCommandPool(Device, &commandPoolCreateInfo, nullptr, &vkCommandPool);
            VK_CHECK(result, "[VulkanRHI] Failed to create command pool!");

            ((VulkanCommandPool*)RhiCommandPool)->SetResource(vkCommandPool);
        }

        // other command pools
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo;
            commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.pNext            = VK_NULL_HANDLE;
            commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            commandPoolCreateInfo.queueFamilyIndex = QueueIndices.graphicsFamily.value();

            for (uint32_t i = 0; i < MaxFramesInFlight; ++i)
            {
                VkResult result = vkCreateCommandPool(Device, &commandPoolCreateInfo, nullptr, &CommandPools[i]);
                VK_CHECK(result, "[VulkanRHI] Failed to create command pool!");
            }
        }
    }

    bool VulkanRHI::CreateCommandPool(const RHICommandPoolCreateInfo* pCreateInfo, RHICommandPool* &pCommandPool)
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkCommandPoolCreateFlags)pCreateInfo->flags;
        createInfo.queueFamilyIndex = pCreateInfo->queueFamilyIndex;

        pCommandPool = new VulkanCommandPool();
        VkCommandPool vkCommandPool;
        VkResult result = vkCreateCommandPool(Device, &createInfo, nullptr, &vkCommandPool);
        ((VulkanCommandPool*)pCommandPool)->SetResource(vkCommandPool);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create command pool!")
    }

    bool VulkanRHI::CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* pCreateInfo, RHIDescriptorPool* & pDescriptorPool)
    {
        int size = pCreateInfo->poolSizeCount;
        std::vector<VkDescriptorPoolSize> descriptorPoolSize(size);
        for (int i = 0; i < size; ++i)
        {
            const auto& rhiDesc = pCreateInfo->pPoolSizes[i];
            auto& vkDesc = descriptorPoolSize[i];

            vkDesc.type = (VkDescriptorType)rhiDesc.type;
            vkDesc.descriptorCount = rhiDesc.descriptorCount;
        };

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkDescriptorPoolCreateFlags)pCreateInfo->flags;
        createInfo.maxSets = pCreateInfo->maxSets;
        createInfo.poolSizeCount = pCreateInfo->poolSizeCount;
        createInfo.pPoolSizes = descriptorPoolSize.data();

        pDescriptorPool = new VulkanDescriptorPool();
        VkDescriptorPool vkDescriptorPool;
        VkResult result = vkCreateDescriptorPool(Device, &createInfo, nullptr, &vkDescriptorPool);
        ((VulkanDescriptorPool*)pDescriptorPool)->SetResource(vkDescriptorPool);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create descriptor pool!")
    }

    bool VulkanRHI::CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* pCreateInfo, RHIDescriptorSetLayout* &pSetLayout)
    {
        //descriptor_set_layout_binding
        int descriptorSetLayoutBindingSize = pCreateInfo->bindingCount;
        std::vector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindingList(descriptorSetLayoutBindingSize);

        int samplerCount = 0;
        for (int i = 0; i < descriptorSetLayoutBindingSize; ++i)
        {
            const auto& rhiDescriptorSetLayoutBindingElement = pCreateInfo->pBindings[i];
            if (rhiDescriptorSetLayoutBindingElement.pImmutableSamplers != nullptr)
            {
                samplerCount += rhiDescriptorSetLayoutBindingElement.descriptorCount;
            }
        }
        std::vector<VkSampler> samplerList(samplerCount);
        int samplerCurrent = 0;

        for (int i = 0; i < descriptorSetLayoutBindingSize; ++i)
        {
            const auto& rhiDescriptorSetLayoutBindingElement = pCreateInfo->pBindings[i];
            auto& vkDescriptorSetLayoutBindingElement = vkDescriptorSetLayoutBindingList[i];

            //sampler
            vkDescriptorSetLayoutBindingElement.pImmutableSamplers = nullptr;
            if (rhiDescriptorSetLayoutBindingElement.pImmutableSamplers)
            {
                vkDescriptorSetLayoutBindingElement.pImmutableSamplers = &samplerList[samplerCurrent];
                for (int i = 0; i < rhiDescriptorSetLayoutBindingElement.descriptorCount; ++i)
                {
                    const auto& rhiSamplerElement = rhiDescriptorSetLayoutBindingElement.pImmutableSamplers[i];
                    auto& vkSamplerElement = samplerList[samplerCurrent];

                    vkSamplerElement = ((VulkanSampler*)rhiSamplerElement)->GetResource();

                    samplerCurrent++;
                };
            }
            vkDescriptorSetLayoutBindingElement.binding = rhiDescriptorSetLayoutBindingElement.binding;
            vkDescriptorSetLayoutBindingElement.descriptorType = (VkDescriptorType)rhiDescriptorSetLayoutBindingElement.descriptorType;
            vkDescriptorSetLayoutBindingElement.descriptorCount = rhiDescriptorSetLayoutBindingElement.descriptorCount;
            vkDescriptorSetLayoutBindingElement.stageFlags = rhiDescriptorSetLayoutBindingElement.stageFlags;
        };

        if (samplerCount != samplerCurrent)
        {
            GAL_CORE_ERROR("[VulkanRHI] sampler_count != sampler_current");
            return false;
        }

        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkDescriptorSetLayoutCreateFlags)pCreateInfo->flags;
        createInfo.bindingCount = pCreateInfo->bindingCount;
        createInfo.pBindings = vkDescriptorSetLayoutBindingList.data();

        pSetLayout = new VulkanDescriptorSetLayout();
        VkDescriptorSetLayout vkDescriptorSetLayout;
        VkResult result = vkCreateDescriptorSetLayout(Device, &createInfo, nullptr, &vkDescriptorSetLayout);
        ((VulkanDescriptorSetLayout*)pSetLayout)->SetResource(vkDescriptorSetLayout);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create descriptor layout!")
    }

    bool VulkanRHI::CreateFence(const RHIFenceCreateInfo* pCreateInfo, RHIFence* &pFence)
    {
        VkFenceCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkFenceCreateFlags)pCreateInfo->flags;

        pFence = new VulkanFence();
        VkFence vkFence;
        VkResult result = vkCreateFence(Device, &createInfo, nullptr, &vkFence);
        ((VulkanFence*)pFence)->SetResource(vkFence);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create fence!")
    }

    bool VulkanRHI::CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo, RHIFramebuffer* &pFramebuffer)
    {
        //image_view
        int imageViewSize = pCreateInfo->attachmentCount;
        std::vector<VkImageView> vkImageViewList(imageViewSize);
        for (int i = 0; i < imageViewSize; ++i)
        {
            const auto& rhiImageViewElement = pCreateInfo->pAttachments[i];
            auto& vkImageViewElement = vkImageViewList[i];

            vkImageViewElement = ((VulkanImageView*)rhiImageViewElement)->GetResource();
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkFramebufferCreateFlags)pCreateInfo->flags;
        createInfo.renderPass = ((VulkanRenderPass*)pCreateInfo->renderPass)->GetResource();
        createInfo.attachmentCount = pCreateInfo->attachmentCount;
        createInfo.pAttachments = vkImageViewList.data();
        createInfo.width = pCreateInfo->width;
        createInfo.height = pCreateInfo->height;
        createInfo.layers = pCreateInfo->layers;

        pFramebuffer = new VulkanFramebuffer();
        VkFramebuffer vkFramebuffer;
        VkResult result = vkCreateFramebuffer(Device, &createInfo, nullptr, &vkFramebuffer);
        ((VulkanFramebuffer*)pFramebuffer)->SetResource(vkFramebuffer);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create framebuffer!")
    }

    bool VulkanRHI::CreateGraphicsPipelines(RHIPipelineCache* pipelineCache, uint32_t createInfoCount, const RHIGraphicsPipelineCreateInfo* pCreateInfo, RHIPipeline* &pPipelines)
    {
        //pipeline_shader_stage_create_info
        int pipelineShaderStageCreateInfoSize = pCreateInfo->stageCount;
        std::vector<VkPipelineShaderStageCreateInfo> vkPipelineShaderStageCreateInfoList(pipelineShaderStageCreateInfoSize);

        int specializationMapEntrySizeTotal = 0;
        int specializationInfoTotal = 0;
        for (int i = 0; i < pipelineShaderStageCreateInfoSize; ++i)
        {
            const auto& rhiPipelineShaderStageCreateInfoElement = pCreateInfo->pStages[i];
            if (rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo != nullptr)
            {
                specializationInfoTotal++;
                specializationMapEntrySizeTotal+= rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->mapEntryCount;
            }
        }
        std::vector<VkSpecializationInfo> vkSpecializationInfoList(specializationInfoTotal);
        std::vector<VkSpecializationMapEntry> vkSpecializationMapEntryList(specializationMapEntrySizeTotal);
        int specializationMapEntryCurrent = 0;
        int specializationInfoCurrent = 0;

        for (int i = 0; i < pipelineShaderStageCreateInfoSize; ++i)
        {
            const auto& rhiPipelineShaderStageCreateInfoElement = pCreateInfo->pStages[i];
            auto& vkPipelineShaderStageCreateInfoElement = vkPipelineShaderStageCreateInfoList[i];

            if (rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo != nullptr)
            {
                vkPipelineShaderStageCreateInfoElement.pSpecializationInfo = &vkSpecializationInfoList[specializationInfoCurrent];

                VkSpecializationInfo vkSpecializationInfo{};
                vkSpecializationInfo.mapEntryCount = rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->mapEntryCount;
                vkSpecializationInfo.pMapEntries = &vkSpecializationMapEntryList[specializationMapEntryCurrent];
                vkSpecializationInfo.dataSize = rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->dataSize;
                vkSpecializationInfo.pData = (const void*)rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->pData;

                //specialization_map_entry
                for (int i = 0; i < rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->mapEntryCount; ++i)
                {
                    const auto& rhiSpecializationMapEntryElement = rhiPipelineShaderStageCreateInfoElement.pSpecializationInfo->pMapEntries[i];
                    auto& vkSpecializationMapEntryElement = vkSpecializationMapEntryList[specializationMapEntryCurrent];

                    vkSpecializationMapEntryElement.constantID = rhiSpecializationMapEntryElement->constantID;
                    vkSpecializationMapEntryElement.offset = rhiSpecializationMapEntryElement->offset;
                    vkSpecializationMapEntryElement.size = rhiSpecializationMapEntryElement->size;

                    specializationMapEntryCurrent++;
                };

                specializationInfoCurrent++;
            }
            else
            {
                vkPipelineShaderStageCreateInfoElement.pSpecializationInfo = nullptr;
            }
            vkPipelineShaderStageCreateInfoElement.sType = (VkStructureType)rhiPipelineShaderStageCreateInfoElement.sType;
            vkPipelineShaderStageCreateInfoElement.pNext = (const void*)rhiPipelineShaderStageCreateInfoElement.pNext;
            vkPipelineShaderStageCreateInfoElement.flags = (VkPipelineShaderStageCreateFlags)rhiPipelineShaderStageCreateInfoElement.flags;
            vkPipelineShaderStageCreateInfoElement.stage = (VkShaderStageFlagBits)rhiPipelineShaderStageCreateInfoElement.stage;
            vkPipelineShaderStageCreateInfoElement.module = ((VulkanShader*)rhiPipelineShaderStageCreateInfoElement.module)->GetResource();
            vkPipelineShaderStageCreateInfoElement.pName = rhiPipelineShaderStageCreateInfoElement.pName;
        };

        if ((specializationMapEntrySizeTotal != specializationMapEntryCurrent)
              || (specializationInfoTotal != specializationInfoCurrent))
        {
            GAL_CORE_ERROR("[VulkanRHI] (specialization_map_entry_size_total == specialization_map_entry_current)&& (specialization_info_total == specialization_info_current)");
            return false;
        }

        //vertex_input_binding_description
        int vertexInputBindingDescriptionSize = pCreateInfo->pVertexInputState->vertexBindingDescriptionCount;
        std::vector<VkVertexInputBindingDescription> vkVertexInputBindingDescriptionList(vertexInputBindingDescriptionSize);
        for (int i = 0; i < vertexInputBindingDescriptionSize; ++i)
        {
            const auto& rhiVertexInputBindingDescriptionElement = pCreateInfo->pVertexInputState->pVertexBindingDescriptions[i];
            auto& vkVertexInputBindingDescriptionElement = vkVertexInputBindingDescriptionList[i];

            vkVertexInputBindingDescriptionElement.binding = rhiVertexInputBindingDescriptionElement.binding;
            vkVertexInputBindingDescriptionElement.stride = rhiVertexInputBindingDescriptionElement.stride;
            vkVertexInputBindingDescriptionElement.inputRate = (VkVertexInputRate)rhiVertexInputBindingDescriptionElement.inputRate;
        };

        //vertex_input_attribute_description
        int vertexInputAttributeDescriptionSize = pCreateInfo->pVertexInputState->vertexAttributeDescriptionCount;
        std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescriptionList(vertexInputAttributeDescriptionSize);
        for (int i = 0; i < vertexInputAttributeDescriptionSize; ++i)
        {
            const auto& rhiVertexInputAttributeDescriptionElement = pCreateInfo->pVertexInputState->pVertexAttributeDescriptions[i];
            auto& vkVertexInputAttributeDescriptionElement = vkVertexInputAttributeDescriptionList[i];

            vkVertexInputAttributeDescriptionElement.location = rhiVertexInputAttributeDescriptionElement.location;
            vkVertexInputAttributeDescriptionElement.binding = rhiVertexInputAttributeDescriptionElement.binding;
            vkVertexInputAttributeDescriptionElement.format = (VkFormat)rhiVertexInputAttributeDescriptionElement.format;
            vkVertexInputAttributeDescriptionElement.offset = rhiVertexInputAttributeDescriptionElement.offset;
        };

        VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo{};
        vkPipelineVertexInputStateCreateInfo.sType = (VkStructureType)pCreateInfo->pVertexInputState->sType;
        vkPipelineVertexInputStateCreateInfo.pNext = (const void*)pCreateInfo->pVertexInputState->pNext;
        vkPipelineVertexInputStateCreateInfo.flags = (VkPipelineVertexInputStateCreateFlags)pCreateInfo->pVertexInputState->flags;
        vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = pCreateInfo->pVertexInputState->vertexBindingDescriptionCount;
        vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescriptionList.data();
        vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = pCreateInfo->pVertexInputState->vertexAttributeDescriptionCount;
        vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescriptionList.data();

        VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo{};
        vkPipelineInputAssemblyStateCreateInfo.sType = (VkStructureType)pCreateInfo->pInputAssemblyState->sType;
        vkPipelineInputAssemblyStateCreateInfo.pNext = (const void*)pCreateInfo->pInputAssemblyState->pNext;
        vkPipelineInputAssemblyStateCreateInfo.flags = (VkPipelineInputAssemblyStateCreateFlags)pCreateInfo->pInputAssemblyState->flags;
        vkPipelineInputAssemblyStateCreateInfo.topology = (VkPrimitiveTopology)pCreateInfo->pInputAssemblyState->topology;
        vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = (VkBool32)pCreateInfo->pInputAssemblyState->primitiveRestartEnable;

        const VkPipelineTessellationStateCreateInfo* vkPipelineTessellationStateCreateInfoPtr = nullptr;
        VkPipelineTessellationStateCreateInfo vkPipelineTessellationStateCreateInfo{};
        if (pCreateInfo->pTessellationState != nullptr)
        {
            vkPipelineTessellationStateCreateInfo.sType = (VkStructureType)pCreateInfo->pTessellationState->sType;
            vkPipelineTessellationStateCreateInfo.pNext = (const void*)pCreateInfo->pTessellationState->pNext;
            vkPipelineTessellationStateCreateInfo.flags = (VkPipelineTessellationStateCreateFlags)pCreateInfo->pTessellationState->flags;
            vkPipelineTessellationStateCreateInfo.patchControlPoints = pCreateInfo->pTessellationState->patchControlPoints;

            vkPipelineTessellationStateCreateInfoPtr = &vkPipelineTessellationStateCreateInfo;
        }

        //viewport
        int viewportSize = pCreateInfo->pViewportState->viewportCount;
        std::vector<VkViewport> vkViewportList(viewportSize);
        for (int i = 0; i < viewportSize; ++i)
        {
            const auto& rhiViewportElement = pCreateInfo->pViewportState->pViewports[i];
            auto& vkViewportElement = vkViewportList[i];

            vkViewportElement.x = rhiViewportElement.x;
            vkViewportElement.y = rhiViewportElement.y;
            vkViewportElement.width = rhiViewportElement.width;
            vkViewportElement.height = rhiViewportElement.height;
            vkViewportElement.minDepth = rhiViewportElement.minDepth;
            vkViewportElement.maxDepth = rhiViewportElement.maxDepth;
        };

        //rect_2d
        int rect2dSize = pCreateInfo->pViewportState->scissorCount;
        std::vector<VkRect2D> vkRect2dList(rect2dSize);
        for (int i = 0; i < rect2dSize; ++i)
        {
            const auto& rhiRect2dElement = pCreateInfo->pViewportState->pScissors[i];
            auto& vkRect2dElement = vkRect2dList[i];

            VkOffset2D offset2d{};
            offset2d.x = vkRect2dElement.offset.x;
            offset2d.y = vkRect2dElement.offset.y;

            VkExtent2D extend2d{};
            extend2d.width = rhiRect2dElement.extent.width;
            extend2d.height = rhiRect2dElement.extent.height;

            vkRect2dElement.offset = offset2d;
            vkRect2dElement.extent = extend2d;
        };

        VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo{};
        vkPipelineViewportStateCreateInfo.sType = (VkStructureType)pCreateInfo->pViewportState->sType;
        vkPipelineViewportStateCreateInfo.pNext = (const void*)pCreateInfo->pViewportState->pNext;
        vkPipelineViewportStateCreateInfo.flags = (VkPipelineViewportStateCreateFlags)pCreateInfo->pViewportState->flags;
        vkPipelineViewportStateCreateInfo.viewportCount = pCreateInfo->pViewportState->viewportCount;
        vkPipelineViewportStateCreateInfo.pViewports = vkViewportList.data();
        vkPipelineViewportStateCreateInfo.scissorCount = pCreateInfo->pViewportState->scissorCount;
        vkPipelineViewportStateCreateInfo.pScissors = vkRect2dList.data();

        VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo{};
        vkPipelineRasterizationStateCreateInfo.sType = (VkStructureType)pCreateInfo->pRasterizationState->sType;
        vkPipelineRasterizationStateCreateInfo.pNext = (const void*)pCreateInfo->pRasterizationState->pNext;
        vkPipelineRasterizationStateCreateInfo.flags = (VkPipelineRasterizationStateCreateFlags)pCreateInfo->pRasterizationState->flags;
        vkPipelineRasterizationStateCreateInfo.depthClampEnable = (VkBool32)pCreateInfo->pRasterizationState->depthClampEnable;
        vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = (VkBool32)pCreateInfo->pRasterizationState->rasterizerDiscardEnable;
        vkPipelineRasterizationStateCreateInfo.polygonMode = (VkPolygonMode)pCreateInfo->pRasterizationState->polygonMode;
        vkPipelineRasterizationStateCreateInfo.cullMode = (VkCullModeFlags)pCreateInfo->pRasterizationState->cullMode;
        vkPipelineRasterizationStateCreateInfo.frontFace = (VkFrontFace)pCreateInfo->pRasterizationState->frontFace;
        vkPipelineRasterizationStateCreateInfo.depthBiasEnable = (VkBool32)pCreateInfo->pRasterizationState->depthBiasEnable;
        vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = pCreateInfo->pRasterizationState->depthBiasConstantFactor;
        vkPipelineRasterizationStateCreateInfo.depthBiasClamp = pCreateInfo->pRasterizationState->depthBiasClamp;
        vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = pCreateInfo->pRasterizationState->depthBiasSlopeFactor;
        vkPipelineRasterizationStateCreateInfo.lineWidth = pCreateInfo->pRasterizationState->lineWidth;

        VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo{};
        vkPipelineMultisampleStateCreateInfo.sType = (VkStructureType)pCreateInfo->pMultisampleState->sType;
        vkPipelineMultisampleStateCreateInfo.pNext = (const void*)pCreateInfo->pMultisampleState->pNext;
        vkPipelineMultisampleStateCreateInfo.flags = (VkPipelineMultisampleStateCreateFlags)pCreateInfo->pMultisampleState->flags;
        vkPipelineMultisampleStateCreateInfo.rasterizationSamples = (VkSampleCountFlagBits)pCreateInfo->pMultisampleState->rasterizationSamples;
        vkPipelineMultisampleStateCreateInfo.sampleShadingEnable = (VkBool32)pCreateInfo->pMultisampleState->sampleShadingEnable;
        vkPipelineMultisampleStateCreateInfo.minSampleShading = pCreateInfo->pMultisampleState->minSampleShading;
        vkPipelineMultisampleStateCreateInfo.pSampleMask = (const RHISampleMask*)pCreateInfo->pMultisampleState->pSampleMask;
        vkPipelineMultisampleStateCreateInfo.alphaToCoverageEnable = (VkBool32)pCreateInfo->pMultisampleState->alphaToCoverageEnable;
        vkPipelineMultisampleStateCreateInfo.alphaToOneEnable = (VkBool32)pCreateInfo->pMultisampleState->alphaToOneEnable;

        VkStencilOpState stencilOpStateFront{};
        stencilOpStateFront.failOp = (VkStencilOp)pCreateInfo->pDepthStencilState->front.failOp;
        stencilOpStateFront.passOp = (VkStencilOp)pCreateInfo->pDepthStencilState->front.passOp;
        stencilOpStateFront.depthFailOp = (VkStencilOp)pCreateInfo->pDepthStencilState->front.depthFailOp;
        stencilOpStateFront.compareOp = (VkCompareOp)pCreateInfo->pDepthStencilState->front.compareOp;
        stencilOpStateFront.compareMask = pCreateInfo->pDepthStencilState->front.compareMask;
        stencilOpStateFront.writeMask = pCreateInfo->pDepthStencilState->front.writeMask;
        stencilOpStateFront.reference = pCreateInfo->pDepthStencilState->front.reference;

        VkStencilOpState stencilOpStateBack{};
        stencilOpStateBack.failOp = (VkStencilOp)pCreateInfo->pDepthStencilState->back.failOp;
        stencilOpStateBack.passOp = (VkStencilOp)pCreateInfo->pDepthStencilState->back.passOp;
        stencilOpStateBack.depthFailOp = (VkStencilOp)pCreateInfo->pDepthStencilState->back.depthFailOp;
        stencilOpStateBack.compareOp = (VkCompareOp)pCreateInfo->pDepthStencilState->back.compareOp;
        stencilOpStateBack.compareMask = pCreateInfo->pDepthStencilState->back.compareMask;
        stencilOpStateBack.writeMask = pCreateInfo->pDepthStencilState->back.writeMask;
        stencilOpStateBack.reference = pCreateInfo->pDepthStencilState->back.reference;


        VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo{};
        vkPipelineDepthStencilStateCreateInfo.sType = (VkStructureType)pCreateInfo->pDepthStencilState->sType;
        vkPipelineDepthStencilStateCreateInfo.pNext = (const void*)pCreateInfo->pDepthStencilState->pNext;
        vkPipelineDepthStencilStateCreateInfo.flags = (VkPipelineDepthStencilStateCreateFlags)pCreateInfo->pDepthStencilState->flags;
        vkPipelineDepthStencilStateCreateInfo.depthTestEnable = (VkBool32)pCreateInfo->pDepthStencilState->depthTestEnable;
        vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = (VkBool32)pCreateInfo->pDepthStencilState->depthWriteEnable;
        vkPipelineDepthStencilStateCreateInfo.depthCompareOp = (VkCompareOp)pCreateInfo->pDepthStencilState->depthCompareOp;
        vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = (VkBool32)pCreateInfo->pDepthStencilState->depthBoundsTestEnable;
        vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = (VkBool32)pCreateInfo->pDepthStencilState->stencilTestEnable;
        vkPipelineDepthStencilStateCreateInfo.front = stencilOpStateFront;
        vkPipelineDepthStencilStateCreateInfo.back = stencilOpStateBack;
        vkPipelineDepthStencilStateCreateInfo.minDepthBounds = pCreateInfo->pDepthStencilState->minDepthBounds;
        vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = pCreateInfo->pDepthStencilState->maxDepthBounds;

        //pipeline_color_blend_attachment_state
        int pipelineColorBlendAttachmentStateSize = pCreateInfo->pColorBlendState->attachmentCount;
        std::vector<VkPipelineColorBlendAttachmentState> vkPipelineColorBlendAttachmentStateList(pipelineColorBlendAttachmentStateSize);
        for (int i = 0; i < pipelineColorBlendAttachmentStateSize; ++i)
        {
            const auto& rhiPipelineColorBlendAttachmentStateElement = pCreateInfo->pColorBlendState->pAttachments[i];
            auto& vkPipelineColorBlendAttachmentStateElement = vkPipelineColorBlendAttachmentStateList[i];

            vkPipelineColorBlendAttachmentStateElement.blendEnable = (VkBool32)rhiPipelineColorBlendAttachmentStateElement.blendEnable;
            vkPipelineColorBlendAttachmentStateElement.srcColorBlendFactor = (VkBlendFactor)rhiPipelineColorBlendAttachmentStateElement.srcColorBlendFactor;
            vkPipelineColorBlendAttachmentStateElement.dstColorBlendFactor = (VkBlendFactor)rhiPipelineColorBlendAttachmentStateElement.dstColorBlendFactor;
            vkPipelineColorBlendAttachmentStateElement.colorBlendOp = (VkBlendOp)rhiPipelineColorBlendAttachmentStateElement.colorBlendOp;
            vkPipelineColorBlendAttachmentStateElement.srcAlphaBlendFactor = (VkBlendFactor)rhiPipelineColorBlendAttachmentStateElement.srcAlphaBlendFactor;
            vkPipelineColorBlendAttachmentStateElement.dstAlphaBlendFactor = (VkBlendFactor)rhiPipelineColorBlendAttachmentStateElement.dstAlphaBlendFactor;
            vkPipelineColorBlendAttachmentStateElement.alphaBlendOp = (VkBlendOp)rhiPipelineColorBlendAttachmentStateElement.alphaBlendOp;
            vkPipelineColorBlendAttachmentStateElement.colorWriteMask = (VkColorComponentFlags)rhiPipelineColorBlendAttachmentStateElement.colorWriteMask;
        };

        VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo{};
        vkPipelineColorBlendStateCreateInfo.sType = (VkStructureType)pCreateInfo->pColorBlendState->sType;
        vkPipelineColorBlendStateCreateInfo.pNext = pCreateInfo->pColorBlendState->pNext;
        vkPipelineColorBlendStateCreateInfo.flags = pCreateInfo->pColorBlendState->flags;
        vkPipelineColorBlendStateCreateInfo.logicOpEnable = pCreateInfo->pColorBlendState->logicOpEnable;
        vkPipelineColorBlendStateCreateInfo.logicOp = (VkLogicOp)pCreateInfo->pColorBlendState->logicOp;
        vkPipelineColorBlendStateCreateInfo.attachmentCount = pCreateInfo->pColorBlendState->attachmentCount;
        vkPipelineColorBlendStateCreateInfo.pAttachments = vkPipelineColorBlendAttachmentStateList.data();
        for (int i = 0; i < 4; ++i)
        {
            vkPipelineColorBlendStateCreateInfo.blendConstants[i] = pCreateInfo->pColorBlendState->blendConstants[i];
        };

        //dynamic_state
        int dynamicStateSize = pCreateInfo->pDynamicState->dynamicStateCount;
        std::vector<VkDynamicState> vkDynamicStateList(dynamicStateSize);
        for (int i = 0; i < dynamicStateSize; ++i)
        {
            const auto& rhiDynamicStateElement = pCreateInfo->pDynamicState->pDynamicStates[i];
            auto& vkDynamicStateElement = vkDynamicStateList[i];

            vkDynamicStateElement = (VkDynamicState)rhiDynamicStateElement;
        };

        VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo{};
        vkPipelineDynamicStateCreateInfo.sType = (VkStructureType)pCreateInfo->pDynamicState->sType;
        vkPipelineDynamicStateCreateInfo.pNext = pCreateInfo->pDynamicState->pNext;
        vkPipelineDynamicStateCreateInfo.flags = (VkPipelineDynamicStateCreateFlags)pCreateInfo->pDynamicState->flags;
        vkPipelineDynamicStateCreateInfo.dynamicStateCount = pCreateInfo->pDynamicState->dynamicStateCount;
        vkPipelineDynamicStateCreateInfo.pDynamicStates = vkDynamicStateList.data();

        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkPipelineCreateFlags)pCreateInfo->flags;
        createInfo.stageCount = pCreateInfo->stageCount;
        createInfo.pStages = vkPipelineShaderStageCreateInfoList.data();
        createInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
        createInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
        createInfo.pTessellationState = vkPipelineTessellationStateCreateInfoPtr;
        createInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
        createInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
        createInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
        createInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
        createInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
        createInfo.pDynamicState = &vkPipelineDynamicStateCreateInfo;
        createInfo.layout = ((VulkanPipelineLayout*)pCreateInfo->layout)->GetResource();
        createInfo.renderPass = ((VulkanRenderPass*)pCreateInfo->renderPass)->GetResource();
        createInfo.subpass = pCreateInfo->subpass;
        if (pCreateInfo->basePipelineHandle != nullptr)
        {
            createInfo.basePipelineHandle = ((VulkanPipeline*)pCreateInfo->basePipelineHandle)->GetResource();
        }
        else
        {
            createInfo.basePipelineHandle = VK_NULL_HANDLE;
        }
        createInfo.basePipelineIndex = pCreateInfo->basePipelineIndex;

        pPipelines = new VulkanPipeline();
        VkPipeline vkPipelines;
        VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;
        if (pipelineCache != nullptr)
        {
            vkPipelineCache = ((VulkanPipelineCache*)pipelineCache)->GetResource();
        }
        VkResult result = vkCreateGraphicsPipelines(Device, vkPipelineCache, createInfoCount, &createInfo, nullptr, &vkPipelines);
        ((VulkanPipeline*)pPipelines)->SetResource(vkPipelines);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create graphics pipeline!")
    }

    bool VulkanRHI::CreateComputePipelines(RHIPipelineCache* pipelineCache, uint32_t createInfoCount, const RHIComputePipelineCreateInfo* pCreateInfos, RHIPipeline*& pPipelines)
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
        if (pCreateInfos->pStages->pSpecializationInfo != nullptr)
        {
            //will be complete soon if needed.
            shaderStageCreateInfo.pSpecializationInfo = nullptr;
        }
        else
        {
            shaderStageCreateInfo.pSpecializationInfo = nullptr;
        }
        shaderStageCreateInfo.sType = (VkStructureType)pCreateInfos->pStages->sType;
        shaderStageCreateInfo.pNext = (const void*)pCreateInfos->pStages->pNext;
        shaderStageCreateInfo.flags = (VkPipelineShaderStageCreateFlags)pCreateInfos->pStages->flags;
        shaderStageCreateInfo.stage = (VkShaderStageFlagBits)pCreateInfos->pStages->stage;
        shaderStageCreateInfo.module = ((VulkanShader*)pCreateInfos->pStages->module)->GetResource();
        shaderStageCreateInfo.pName = pCreateInfos->pStages->pName;

        VkComputePipelineCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfos->sType;
        createInfo.pNext = (const void*)pCreateInfos->pNext;
        createInfo.flags = (VkPipelineCreateFlags)pCreateInfos->flags;
        createInfo.stage = shaderStageCreateInfo;
        createInfo.layout = ((VulkanPipelineLayout*)pCreateInfos->layout)->GetResource();;
        if (pCreateInfos->basePipelineHandle != nullptr)
        {
            createInfo.basePipelineHandle = ((VulkanPipeline*)pCreateInfos->basePipelineHandle)->GetResource();
        }
        else
        {
            createInfo.basePipelineHandle = VK_NULL_HANDLE;
        }
        createInfo.basePipelineIndex = pCreateInfos->basePipelineIndex;

        pPipelines = new VulkanPipeline();
        VkPipeline vkPipelines;
        VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;
        if (pipelineCache != nullptr)
        {
            vkPipelineCache = ((VulkanPipelineCache*)pipelineCache)->GetResource();
        }
        VkResult result = vkCreateComputePipelines(Device, vkPipelineCache, createInfoCount, &createInfo, nullptr, &vkPipelines);
        ((VulkanPipeline*)pPipelines)->SetResource(vkPipelines);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create compute pipelines!")
    }

    bool VulkanRHI::CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* pCreateInfo, RHIPipelineLayout* &pPipelineLayout)
    {
        //descriptor_set_layout
        int descriptorSetLayoutSize = pCreateInfo->setLayoutCount;
        std::vector<VkDescriptorSetLayout> vkDescriptorSetLayoutList(descriptorSetLayoutSize);
        for (int i = 0; i < descriptorSetLayoutSize; ++i)
        {
            const auto& rhiDescriptorSetLayoutElement = pCreateInfo->pSetLayouts[i];
            auto& vkDescriptorSetLayoutElement = vkDescriptorSetLayoutList[i];

            vkDescriptorSetLayoutElement = ((VulkanDescriptorSetLayout*)rhiDescriptorSetLayoutElement)->GetResource();
        };

        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkPipelineLayoutCreateFlags)pCreateInfo->flags;
        createInfo.setLayoutCount = pCreateInfo->setLayoutCount;
        createInfo.pSetLayouts = vkDescriptorSetLayoutList.data();

        pPipelineLayout = new VulkanPipelineLayout();
        VkPipelineLayout vkPipelineLayout;
        VkResult result = vkCreatePipelineLayout(Device, &createInfo, nullptr, &vkPipelineLayout);
        ((VulkanPipelineLayout*)pPipelineLayout)->SetResource(vkPipelineLayout);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create pipeline layout!")
    }

    bool VulkanRHI::CreateRenderPass(const RHIRenderPassCreateInfo* pCreateInfo, RHIRenderPass* &pRenderPass)
    {
        // attachment convert
        std::vector<VkAttachmentDescription> vkAttachments(pCreateInfo->attachmentCount);
        for (int i = 0; i < pCreateInfo->attachmentCount; ++i)
        {
            const auto& rhiDesc = pCreateInfo->pAttachments[i];
            auto& vkDesc = vkAttachments[i];

            vkDesc.flags = (VkAttachmentDescriptionFlags)(rhiDesc).flags;
            vkDesc.format = (VkFormat)(rhiDesc).format;
            vkDesc.samples = (VkSampleCountFlagBits)(rhiDesc).samples;
            vkDesc.loadOp = (VkAttachmentLoadOp)(rhiDesc).loadOp;
            vkDesc.storeOp = (VkAttachmentStoreOp)(rhiDesc).storeOp;
            vkDesc.stencilLoadOp = (VkAttachmentLoadOp)(rhiDesc).stencilLoadOp;
            vkDesc.stencilStoreOp = (VkAttachmentStoreOp)(rhiDesc).stencilStoreOp;
            vkDesc.initialLayout = (VkImageLayout)(rhiDesc).initialLayout;
            vkDesc.finalLayout = (VkImageLayout)(rhiDesc).finalLayout;
        };

        // subpass convert
        int totalAttachmentReference = 0;
        for (int i = 0; i < pCreateInfo->subpassCount; i++)
        {
            const auto& rhiDesc = pCreateInfo->pSubpasses[i];
            totalAttachmentReference += rhiDesc.inputAttachmentCount; // pInputAttachments
            totalAttachmentReference += rhiDesc.colorAttachmentCount; // pColorAttachments
            if (rhiDesc.pDepthStencilAttachment != nullptr)
            {
                totalAttachmentReference += rhiDesc.colorAttachmentCount; // pDepthStencilAttachment
            }
            if (rhiDesc.pResolveAttachments != nullptr)
            {
                totalAttachmentReference += rhiDesc.colorAttachmentCount; // pResolveAttachments
            }
        }
        std::vector<VkSubpassDescription> vkSubpassDescription(pCreateInfo->subpassCount);
        std::vector<VkAttachmentReference> vkAttachmentReference(totalAttachmentReference);
        int                                currentAttachmentReference = 0;
        for (int i = 0; i < pCreateInfo->subpassCount; ++i)
        {
            const auto& rhiDesc = pCreateInfo->pSubpasses[i];
            auto& vkDesc = vkSubpassDescription[i];

            vkDesc.flags = (VkSubpassDescriptionFlags)(rhiDesc).flags;
            vkDesc.pipelineBindPoint = (VkPipelineBindPoint)(rhiDesc).pipelineBindPoint;
            vkDesc.preserveAttachmentCount = (rhiDesc).preserveAttachmentCount;
            vkDesc.pPreserveAttachments = (const uint32_t*)(rhiDesc).pPreserveAttachments;

            vkDesc.inputAttachmentCount = (rhiDesc).inputAttachmentCount;
            vkDesc.pInputAttachments = &vkAttachmentReference[currentAttachmentReference];
            for (int i = 0; i < (rhiDesc).inputAttachmentCount; i++)
            {
                const auto& rhiAttachmentReferenceInput = (rhiDesc).pInputAttachments[i];
                auto&       vkAttachmentReferenceInput  = vkAttachmentReference[currentAttachmentReference];

                vkAttachmentReferenceInput.attachment = rhiAttachmentReferenceInput.attachment;
                vkAttachmentReferenceInput.layout = (VkImageLayout)(rhiAttachmentReferenceInput.layout);

                currentAttachmentReference += 1;
            };

            vkDesc.colorAttachmentCount = (rhiDesc).colorAttachmentCount;
            vkDesc.pColorAttachments = &vkAttachmentReference[currentAttachmentReference];
            for (int i = 0; i < (rhiDesc).colorAttachmentCount; ++i)
            {
                const auto& rhiAttachmentReferenceColor = (rhiDesc).pColorAttachments[i];
                auto&       vkAttachmentReferenceColor  = vkAttachmentReference[currentAttachmentReference];

                vkAttachmentReferenceColor.attachment = rhiAttachmentReferenceColor.attachment;
                vkAttachmentReferenceColor.layout = (VkImageLayout)(rhiAttachmentReferenceColor.layout);

                currentAttachmentReference += 1;
            };

            if (rhiDesc.pResolveAttachments != nullptr)
            {
                vkDesc.pResolveAttachments = &vkAttachmentReference[currentAttachmentReference];
                for (int i = 0; i < (rhiDesc).colorAttachmentCount; ++i)
                {
                    const auto& rhiAttachmentReferenceResolve = (rhiDesc).pResolveAttachments[i];
                    auto&       vkAttachmentReferenceResolve  = vkAttachmentReference[currentAttachmentReference];

                    vkAttachmentReferenceResolve.attachment = rhiAttachmentReferenceResolve.attachment;
                    vkAttachmentReferenceResolve.layout = (VkImageLayout)(rhiAttachmentReferenceResolve.layout);

                    currentAttachmentReference += 1;
                };
            }

            if (rhiDesc.pDepthStencilAttachment != nullptr)
            {
                vkDesc.pDepthStencilAttachment = &vkAttachmentReference[currentAttachmentReference];
                for (int i = 0; i < (rhiDesc).colorAttachmentCount; ++i)
                {
                    const auto& rhiAttachmentReferenceDepth = (rhiDesc).pDepthStencilAttachment[i];
                    auto&       vkAttachmentReferenceDepth  = vkAttachmentReference[currentAttachmentReference];

                    vkAttachmentReferenceDepth.attachment = rhiAttachmentReferenceDepth.attachment;
                    vkAttachmentReferenceDepth.layout = (VkImageLayout)(rhiAttachmentReferenceDepth.layout);

                    currentAttachmentReference += 1;
                };
            };
        };
        if (currentAttachmentReference != totalAttachmentReference)
        {
            GAL_CORE_ERROR("[VulkanRHI] currentAttachmentReference != totalAttachmentReference");
            return false;
        }

        std::vector<VkSubpassDependency> vkSubpassDepandecy(pCreateInfo->dependencyCount);
        for (int i = 0; i < pCreateInfo->dependencyCount; ++i)
        {
            const auto& rhiDesc = pCreateInfo->pDependencies[i];
            auto& vkDesc = vkSubpassDepandecy[i];

            vkDesc.srcSubpass = rhiDesc.srcSubpass;
            vkDesc.dstSubpass = rhiDesc.dstSubpass;
            vkDesc.srcStageMask = (VkPipelineStageFlags)(rhiDesc).srcStageMask;
            vkDesc.dstStageMask = (VkPipelineStageFlags)(rhiDesc).dstStageMask;
            vkDesc.srcAccessMask = (VkAccessFlags)(rhiDesc).srcAccessMask;
            vkDesc.dstAccessMask = (VkAccessFlags)(rhiDesc).dstAccessMask;
            vkDesc.dependencyFlags = (VkDependencyFlags)(rhiDesc).dependencyFlags;
        };

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkRenderPassCreateFlags)pCreateInfo->flags;
        createInfo.attachmentCount = pCreateInfo->attachmentCount;
        createInfo.pAttachments = vkAttachments.data();
        createInfo.subpassCount = pCreateInfo->subpassCount;
        createInfo.pSubpasses = vkSubpassDescription.data();
        createInfo.dependencyCount = pCreateInfo->dependencyCount;
        createInfo.pDependencies = vkSubpassDepandecy.data();

        pRenderPass = new VulkanRenderPass();
        VkRenderPass vkRenderPass;
        VkResult result = vkCreateRenderPass(Device, &createInfo, nullptr, &vkRenderPass);
        ((VulkanRenderPass*)pRenderPass)->SetResource(vkRenderPass);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create render pass!")
    }

    bool VulkanRHI::CreateSampler(const RHISamplerCreateInfo* pCreateInfo, RHISampler* &pSampler)
    {
        VkSamplerCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = (const void*)pCreateInfo->pNext;
        createInfo.flags = (VkSamplerCreateFlags)pCreateInfo->flags;
        createInfo.magFilter = (VkFilter)pCreateInfo->magFilter;
        createInfo.minFilter = (VkFilter)pCreateInfo->minFilter;
        createInfo.mipmapMode = (VkSamplerMipmapMode)pCreateInfo->mipmapMode;
        createInfo.addressModeU = (VkSamplerAddressMode)pCreateInfo->addressModeU;
        createInfo.addressModeV = (VkSamplerAddressMode)pCreateInfo->addressModeV;
        createInfo.addressModeW = (VkSamplerAddressMode)pCreateInfo->addressModeW;
        createInfo.mipLodBias = pCreateInfo->mipLodBias;
        createInfo.anisotropyEnable = (VkBool32)pCreateInfo->anisotropyEnable;
        createInfo.maxAnisotropy = pCreateInfo->maxAnisotropy;
        createInfo.compareEnable = (VkBool32)pCreateInfo->compareEnable;
        createInfo.compareOp = (VkCompareOp)pCreateInfo->compareOp;
        createInfo.minLod = pCreateInfo->minLod;
        createInfo.maxLod = pCreateInfo->maxLod;
        createInfo.borderColor = (VkBorderColor)pCreateInfo->borderColor;
        createInfo.unnormalizedCoordinates = (VkBool32)pCreateInfo->unnormalizedCoordinates;

        pSampler = new VulkanSampler();
        VkSampler vkSampler;
        VkResult result = vkCreateSampler(Device, &createInfo, nullptr, &vkSampler);
        ((VulkanSampler*)pSampler)->SetResource(vkSampler);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create sample!")
    }

    bool VulkanRHI::CreateSemaphore(const RHISemaphoreCreateInfo* pCreateInfo, RHISemaphore* &pSemaphore)
    {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType = (VkStructureType)pCreateInfo->sType;
        createInfo.pNext = pCreateInfo->pNext;
        createInfo.flags = (VkSemaphoreCreateFlags)pCreateInfo->flags;

        pSemaphore = new VulkanSemaphore();
        VkSemaphore vkSemaphore;
        VkResult result = vkCreateSemaphore(Device, &createInfo, nullptr, &vkSemaphore);
        ((VulkanSemaphore*)pSemaphore)->SetResource(vkSemaphore);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create semaphore!")
    }

    bool VulkanRHI::WaitForFencesPfn(uint32_t fenceCount, RHIFence* const* pFences, RHIBool32 waitAll, uint64_t timeout)
    {
        //fence
        int fenceSize = fenceCount;
        std::vector<VkFence> vkFenceList(fenceSize);
        for (int i = 0; i < fenceSize; ++i)
        {
            const auto& rhiFenceElement = pFences[i];
            auto& vkFenceElement = vkFenceList[i];

            vkFenceElement = ((VulkanFence*)rhiFenceElement)->GetResource();
        };

        VkResult result = _vkWaitForFences(Device, fenceCount, vkFenceList.data(), waitAll, timeout);
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to wait for fences!")
    }

    bool VulkanRHI::ResetFencesPfn(uint32_t fenceCount, RHIFence* const* pFences)
    {
        //fence
        int fenceSize = fenceCount;
        std::vector<VkFence> vkFenceList(fenceSize);
        for (int i = 0; i < fenceSize; ++i)
        {
            const auto& rhiFenceElement = pFences[i];
            auto& vkFenceElement = vkFenceList[i];

            vkFenceElement = ((VulkanFence*)rhiFenceElement)->GetResource();
        };

        VkResult result = _vkResetFences(Device, fenceCount, vkFenceList.data());
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to reset fences!")
    }

    bool VulkanRHI::ResetCommandPoolPfn(RHICommandPool* commandPool, RHICommandPoolResetFlags flags)
    {
        VkResult result = _vkResetCommandPool(Device, ((VulkanCommandPool*)commandPool)->GetResource(), (VkCommandPoolResetFlags)flags);
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to reset command pool!")
    }

    bool VulkanRHI::BeginCommandBufferPfn(RHICommandBuffer* commandBuffer, const RHICommandBufferBeginInfo* pBeginInfo)
    {
        VkCommandBufferInheritanceInfo* commandBufferInheritanceInfoPtr = nullptr;
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo{};
        if (pBeginInfo->pInheritanceInfo != nullptr)
        {
            commandBufferInheritanceInfo.sType = (VkStructureType)pBeginInfo->pInheritanceInfo->sType;
            commandBufferInheritanceInfo.pNext = (const void*)pBeginInfo->pInheritanceInfo->pNext;
            commandBufferInheritanceInfo.renderPass = ((VulkanRenderPass*)pBeginInfo->pInheritanceInfo->renderPass)->GetResource();
            commandBufferInheritanceInfo.subpass = pBeginInfo->pInheritanceInfo->subpass;
            commandBufferInheritanceInfo.framebuffer = ((VulkanFramebuffer*)pBeginInfo->pInheritanceInfo->framebuffer)->GetResource();
            commandBufferInheritanceInfo.occlusionQueryEnable = (VkBool32)pBeginInfo->pInheritanceInfo->occlusionQueryEnable;
            commandBufferInheritanceInfo.queryFlags = (VkQueryControlFlags)pBeginInfo->pInheritanceInfo->queryFlags;
            commandBufferInheritanceInfo.pipelineStatistics = (VkQueryPipelineStatisticFlags)pBeginInfo->pInheritanceInfo->pipelineStatistics;

            commandBufferInheritanceInfoPtr = &commandBufferInheritanceInfo;
        }

        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = (VkStructureType)pBeginInfo->sType;
        commandBufferBeginInfo.pNext = (const void*)pBeginInfo->pNext;
        commandBufferBeginInfo.flags = (VkCommandBufferUsageFlags)pBeginInfo->flags;
        commandBufferBeginInfo.pInheritanceInfo = commandBufferInheritanceInfoPtr;
        VkResult result = _vkBeginCommandBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource(), &commandBufferBeginInfo);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to begin command buffer!")
    }

    bool VulkanRHI::EndCommandBufferPfn(RHICommandBuffer* commandBuffer)
    {
        VkResult result = _vkEndCommandBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource());
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to end command buffer!")
    }

    void VulkanRHI::CmdBeginRenderPassPfn(RHICommandBuffer* commandBuffer, const RHIRenderPassBeginInfo* pRenderPassBegin, RHISubpassContents contents)
    {
        VkOffset2D offset2d{};
        offset2d.x = pRenderPassBegin->renderArea.offset.x;
        offset2d.y = pRenderPassBegin->renderArea.offset.y;

        VkExtent2D extent2d{};
        extent2d.width = pRenderPassBegin->renderArea.extent.width;
        extent2d.height = pRenderPassBegin->renderArea.extent.height;

        VkRect2D rect2d{};
        rect2d.offset = offset2d;
        rect2d.extent = extent2d;

        //clear_values
        int clearValueSize = pRenderPassBegin->clearValueCount;
        std::vector<VkClearValue> vkClearValueList(clearValueSize);
        for (int i = 0; i < clearValueSize; ++i)
        {
            const auto& rhiClearValueElement = pRenderPassBegin->pClearValues[i];
            auto& vkClearValueElement = vkClearValueList[i];

            VkClearColorValue vkClearColorValue;
            vkClearColorValue.float32[0] = rhiClearValueElement.color.float32[0];
            vkClearColorValue.float32[1] = rhiClearValueElement.color.float32[1];
            vkClearColorValue.float32[2] = rhiClearValueElement.color.float32[2];
            vkClearColorValue.float32[3] = rhiClearValueElement.color.float32[3];
            vkClearColorValue.int32[0] = rhiClearValueElement.color.int32[0];
            vkClearColorValue.int32[1] = rhiClearValueElement.color.int32[1];
            vkClearColorValue.int32[2] = rhiClearValueElement.color.int32[2];
            vkClearColorValue.int32[3] = rhiClearValueElement.color.int32[3];
            vkClearColorValue.uint32[0] = rhiClearValueElement.color.uint32[0];
            vkClearColorValue.uint32[1] = rhiClearValueElement.color.uint32[1];
            vkClearColorValue.uint32[2] = rhiClearValueElement.color.uint32[2];
            vkClearColorValue.uint32[3] = rhiClearValueElement.color.uint32[3];

            VkClearDepthStencilValue vkClearDepthStencilValue;
            vkClearDepthStencilValue.depth = rhiClearValueElement.depthStencil.depth;
            vkClearDepthStencilValue.stencil = rhiClearValueElement.depthStencil.stencil;

            vkClearValueElement.color = vkClearColorValue;
            vkClearValueElement.depthStencil = vkClearDepthStencilValue;

        };

        VkRenderPassBeginInfo vkRenderPassBeginInfo{};
        vkRenderPassBeginInfo.sType = (VkStructureType)pRenderPassBegin->sType;
        vkRenderPassBeginInfo.pNext = pRenderPassBegin->pNext;
        vkRenderPassBeginInfo.renderPass = ((VulkanRenderPass*)pRenderPassBegin->renderPass)->GetResource();
        vkRenderPassBeginInfo.framebuffer = ((VulkanFramebuffer*)pRenderPassBegin->framebuffer)->GetResource();
        vkRenderPassBeginInfo.renderArea = rect2d;
        vkRenderPassBeginInfo.clearValueCount = pRenderPassBegin->clearValueCount;
        vkRenderPassBeginInfo.pClearValues = vkClearValueList.data();

        return _vkCmdBeginRenderPass(((VulkanCommandBuffer*)commandBuffer)->GetResource(), &vkRenderPassBeginInfo, (VkSubpassContents)contents);
    }

    void VulkanRHI::CmdNextSubpassPfn(RHICommandBuffer* commandBuffer, RHISubpassContents contents)
    {
        return _vkCmdNextSubpass(((VulkanCommandBuffer*)commandBuffer)->GetResource(), ((VkSubpassContents)contents));
    }

    void VulkanRHI::CmdEndRenderPassPfn(RHICommandBuffer* commandBuffer)
    {
        return _vkCmdEndRenderPass(((VulkanCommandBuffer*)commandBuffer)->GetResource());
    }

    void VulkanRHI::CmdBindPipelinePfn(RHICommandBuffer* commandBuffer, RHIPipelineBindPoint pipelineBindPoint, RHIPipeline* pipeline)
    {
        return _vkCmdBindPipeline(((VulkanCommandBuffer*)commandBuffer)->GetResource(), (VkPipelineBindPoint)pipelineBindPoint, ((VulkanPipeline*)pipeline)->GetResource());
    }

    void VulkanRHI::CmdSetViewportPfn(RHICommandBuffer* commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const RHIViewport* pViewports)
    {
        //viewport
        int viewportSize = viewportCount;
        std::vector<VkViewport> vkViewportList(viewportSize);
        for (int i = 0; i < viewportSize; ++i)
        {
            const auto& rhiViewportElement = pViewports[i];
            auto& vkViewportElement = vkViewportList[i];

            vkViewportElement.x = rhiViewportElement.x;
            vkViewportElement.y = rhiViewportElement.y;
            vkViewportElement.width = rhiViewportElement.width;
            vkViewportElement.height = rhiViewportElement.height;
            vkViewportElement.minDepth = rhiViewportElement.minDepth;
            vkViewportElement.maxDepth = rhiViewportElement.maxDepth;
        };

        return _vkCmdSetViewport(((VulkanCommandBuffer*)commandBuffer)->GetResource(), firstViewport, viewportCount, vkViewportList.data());
    }

    void VulkanRHI::CmdSetScissorPfn(RHICommandBuffer* commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const RHIRect2D* pScissors)
    {
        //rect_2d
        int rect2dSize = scissorCount;
        std::vector<VkRect2D> vkRect2dList(rect2dSize);
        for (int i = 0; i < rect2dSize; ++i)
        {
            const auto& rhiRect2dElement = pScissors[i];
            auto& vkRect2dElement = vkRect2dList[i];

            VkOffset2D offset2d{};
            offset2d.x = rhiRect2dElement.offset.x;
            offset2d.y = rhiRect2dElement.offset.y;

            VkExtent2D extent2d{};
            extent2d.width = rhiRect2dElement.extent.width;
            extent2d.height = rhiRect2dElement.extent.height;

            vkRect2dElement.offset = (VkOffset2D)offset2d;
            vkRect2dElement.extent = (VkExtent2D)extent2d;

        };

        return _vkCmdSetScissor(((VulkanCommandBuffer*)commandBuffer)->GetResource(), firstScissor, scissorCount, vkRect2dList.data());
    }

    void VulkanRHI::CmdBindVertexBuffersPfn(
        RHICommandBuffer* commandBuffer,
        uint32_t firstBinding,
        uint32_t bindingCount,
        RHIBuffer* const* pBuffers,
        const RHIDeviceSize* pOffsets)
    {
        //buffer
        int bufferSize = bindingCount;
        std::vector<VkBuffer> vkBufferList(bufferSize);
        for (int i = 0; i < bufferSize; ++i)
        {
            const auto& rhiBufferElement = pBuffers[i];
            auto& vkBufferElement = vkBufferList[i];

            vkBufferElement = ((VulkanBuffer*)rhiBufferElement)->GetResource();
        };

        //offset
        int offsetSize = bindingCount;
        std::vector<VkDeviceSize> vkDeviceSizeList(offsetSize);
        for (int i = 0; i < offsetSize; ++i)
        {
            const auto& rhiOffsetElement = pOffsets[i];
            auto& vkOffsetElement = vkDeviceSizeList[i];

            vkOffsetElement = rhiOffsetElement;
        };

        return _vkCmdBindVertexBuffers(((VulkanCommandBuffer*)commandBuffer)->GetResource(), firstBinding, bindingCount, vkBufferList.data(), vkDeviceSizeList.data());
    }

    void VulkanRHI::CmdBindIndexBufferPfn(RHICommandBuffer* commandBuffer, RHIBuffer* buffer, RHIDeviceSize offset, RHIIndexType indexType)
    {
        return _vkCmdBindIndexBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource(), ((VulkanBuffer*)buffer)->GetResource(), (VkDeviceSize)offset, (VkIndexType)indexType);
    }

    void VulkanRHI::CmdBindDescriptorSetsPfn(
        RHICommandBuffer* commandBuffer,
        RHIPipelineBindPoint pipelineBindPoint,
        RHIPipelineLayout* layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const RHIDescriptorSet* const* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets)
    {
        //descriptor_set
        int descriptorSetSize = descriptorSetCount;
        std::vector<VkDescriptorSet> vkDescriptorSetList(descriptorSetSize);
        for (int i = 0; i < descriptorSetSize; ++i)
        {
            const auto& rhiDescriptorSetElement = pDescriptorSets[i];
            auto& vkDescriptorSetElement = vkDescriptorSetList[i];

            vkDescriptorSetElement = ((VulkanDescriptorSet*)rhiDescriptorSetElement)->GetResource();
        };

        //offset
        int offsetSize = dynamicOffsetCount;
        std::vector<uint32_t> vkOffsetList(offsetSize);
        for (int i = 0; i < offsetSize; ++i)
        {
            const auto& rhiOffsetElement = pDynamicOffsets[i];
            auto& vkOffsetElement = vkOffsetList[i];

            vkOffsetElement = rhiOffsetElement;
        };

        return _vkCmdBindDescriptorSets(
            ((VulkanCommandBuffer*)commandBuffer)->GetResource(),
            (VkPipelineBindPoint)pipelineBindPoint,
            ((VulkanPipelineLayout*)layout)->GetResource(),
            firstSet, descriptorSetCount,
            vkDescriptorSetList.data(),
            dynamicOffsetCount,
            vkOffsetList.data());
    }

    void VulkanRHI::CmdDrawIndexedPfn(RHICommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        return _vkCmdDrawIndexed(((VulkanCommandBuffer*)commandBuffer)->GetResource(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanRHI::CmdClearAttachmentsPfn(
        RHICommandBuffer* commandBuffer,
        uint32_t attachmentCount,
        const RHIClearAttachment* pAttachments,
        uint32_t rectCount,
        const RHIClearRect* pRects)
    {
        //clear_attachment
        int clearAttachmentSize = attachmentCount;
        std::vector<VkClearAttachment> vkClearAttachmentList(clearAttachmentSize);
        for (int i = 0; i < clearAttachmentSize; ++i)
        {
            const auto& rhiClearAttachmentElement = pAttachments[i];
            auto& vkClearAttachmentElement = vkClearAttachmentList[i];

            VkClearColorValue vkClearColorValue;
            vkClearColorValue.float32[0] = rhiClearAttachmentElement.clearValue.color.float32[0];
            vkClearColorValue.float32[1] = rhiClearAttachmentElement.clearValue.color.float32[1];
            vkClearColorValue.float32[2] = rhiClearAttachmentElement.clearValue.color.float32[2];
            vkClearColorValue.float32[3] = rhiClearAttachmentElement.clearValue.color.float32[3];
            vkClearColorValue.int32[0] = rhiClearAttachmentElement.clearValue.color.int32[0];
            vkClearColorValue.int32[1] = rhiClearAttachmentElement.clearValue.color.int32[1];
            vkClearColorValue.int32[2] = rhiClearAttachmentElement.clearValue.color.int32[2];
            vkClearColorValue.int32[3] = rhiClearAttachmentElement.clearValue.color.int32[3];
            vkClearColorValue.uint32[0] = rhiClearAttachmentElement.clearValue.color.uint32[0];
            vkClearColorValue.uint32[1] = rhiClearAttachmentElement.clearValue.color.uint32[1];
            vkClearColorValue.uint32[2] = rhiClearAttachmentElement.clearValue.color.uint32[2];
            vkClearColorValue.uint32[3] = rhiClearAttachmentElement.clearValue.color.uint32[3];

            VkClearDepthStencilValue vkClearDepthStencilValue;
            vkClearDepthStencilValue.depth = rhiClearAttachmentElement.clearValue.depthStencil.depth;
            vkClearDepthStencilValue.stencil = rhiClearAttachmentElement.clearValue.depthStencil.stencil;

            vkClearAttachmentElement.clearValue.color = vkClearColorValue;
            vkClearAttachmentElement.clearValue.depthStencil = vkClearDepthStencilValue;
            vkClearAttachmentElement.aspectMask = rhiClearAttachmentElement.aspectMask;
            vkClearAttachmentElement.colorAttachment = rhiClearAttachmentElement.colorAttachment;
        };

        //clear_rect
        int clearRectSize = rectCount;
        std::vector<VkClearRect> vkClearRectList(clearRectSize);
        for (int i = 0; i < clearRectSize; ++i)
        {
            const auto& rhiClearRectElement = pRects[i];
            auto& vkClearRectElement = vkClearRectList[i];

            VkOffset2D offset2d{};
            offset2d.x = rhiClearRectElement.rect.offset.x;
            offset2d.y = rhiClearRectElement.rect.offset.y;

            VkExtent2D extent2d{};
            extent2d.width = rhiClearRectElement.rect.extent.width;
            extent2d.height = rhiClearRectElement.rect.extent.height;

            vkClearRectElement.rect.offset = (VkOffset2D)offset2d;
            vkClearRectElement.rect.extent = (VkExtent2D)extent2d;
            vkClearRectElement.baseArrayLayer = rhiClearRectElement.baseArrayLayer;
            vkClearRectElement.layerCount = rhiClearRectElement.layerCount;
        };

        return _vkCmdClearAttachments(
            ((VulkanCommandBuffer*)commandBuffer)->GetResource(),
            attachmentCount,
            vkClearAttachmentList.data(),
            rectCount,
            vkClearRectList.data());
    }

    bool VulkanRHI::BeginCommandBuffer(RHICommandBuffer* commandBuffer, const RHICommandBufferBeginInfo* pBeginInfo)
    {
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo{};
        const VkCommandBufferInheritanceInfo* commandBufferInheritanceInfoPtr = nullptr;
        if (pBeginInfo->pInheritanceInfo != nullptr)
        {
            commandBufferInheritanceInfo.sType = (VkStructureType)(pBeginInfo->pInheritanceInfo->sType);
            commandBufferInheritanceInfo.pNext = (const void*)pBeginInfo->pInheritanceInfo->pNext;
            commandBufferInheritanceInfo.renderPass = ((VulkanRenderPass*)pBeginInfo->pInheritanceInfo->renderPass)->GetResource();
            commandBufferInheritanceInfo.subpass = pBeginInfo->pInheritanceInfo->subpass;
            commandBufferInheritanceInfo.framebuffer = ((VulkanFramebuffer*)(pBeginInfo->pInheritanceInfo->framebuffer))->GetResource();
            commandBufferInheritanceInfo.occlusionQueryEnable = (VkBool32)pBeginInfo->pInheritanceInfo->occlusionQueryEnable;
            commandBufferInheritanceInfo.queryFlags = (VkQueryControlFlags)pBeginInfo->pInheritanceInfo->queryFlags;
            commandBufferInheritanceInfo.pipelineStatistics = (VkQueryPipelineStatisticFlags)pBeginInfo->pInheritanceInfo->pipelineStatistics;

            commandBufferInheritanceInfoPtr = &commandBufferInheritanceInfo;
        }

        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = (VkStructureType)pBeginInfo->sType;
        commandBufferBeginInfo.pNext = (const void*)pBeginInfo->pNext;
        commandBufferBeginInfo.flags = (VkCommandBufferUsageFlags)pBeginInfo->flags;
        commandBufferBeginInfo.pInheritanceInfo = commandBufferInheritanceInfoPtr;

        VkResult result = vkBeginCommandBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource(), &commandBufferBeginInfo);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to begin command buffer")
    }

    bool VulkanRHI::EndCommandBuffer(RHICommandBuffer* commandBuffer)
    {
        VkResult result = vkEndCommandBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource());
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to end command buffer")
    }

    void VulkanRHI::UpdateDescriptorSets(
        uint32_t descriptorWriteCount,
        const RHIWriteDescriptorSet* pDescriptorWrites,
        uint32_t descriptorCopyCount,
        const RHICopyDescriptorSet* pDescriptorCopies)
    {
        //write_descriptor_set
        int writeDescriptorSetSize = descriptorWriteCount;
        std::vector<VkWriteDescriptorSet> vkWriteDescriptorSetList(writeDescriptorSetSize);
        int imageInfoCount = 0;
        int bufferInfoCount = 0;
        for (int i = 0; i < writeDescriptorSetSize; ++i)
        {
            const auto& rhiWriteDescriptorSetElement = pDescriptorWrites[i];
            if (rhiWriteDescriptorSetElement.pImageInfo != nullptr)
            {
                imageInfoCount++;
            }
            if (rhiWriteDescriptorSetElement.pBufferInfo != nullptr)
            {
                bufferInfoCount++;
            }
        }
        std::vector<VkDescriptorImageInfo> vkDescriptorImageInfoList(imageInfoCount);
        std::vector<VkDescriptorBufferInfo> vkDescriptorBufferInfoList(bufferInfoCount);
        int imageInfoCurrent = 0;
        int bufferInfoCurrent = 0;

        for (int i = 0; i < writeDescriptorSetSize; ++i)
        {
            const auto& rhiWriteDescriptorSetElement = pDescriptorWrites[i];
            auto& vkWriteDescriptorSetElement = vkWriteDescriptorSetList[i];

            const VkDescriptorImageInfo* vkDescriptorImageInfoPtr = nullptr;
            if (rhiWriteDescriptorSetElement.pImageInfo != nullptr)
            {
                auto& vkDescriptorImageInfo = vkDescriptorImageInfoList[imageInfoCurrent];
                if (rhiWriteDescriptorSetElement.pImageInfo->sampler == nullptr)
                {
                    vkDescriptorImageInfo.sampler = nullptr;
                }
                else
                {
                    vkDescriptorImageInfo.sampler = ((VulkanSampler*)rhiWriteDescriptorSetElement.pImageInfo->sampler)->GetResource();
                }
                vkDescriptorImageInfo.imageView = ((VulkanImageView*)rhiWriteDescriptorSetElement.pImageInfo->imageView)->GetResource();
                vkDescriptorImageInfo.imageLayout = (VkImageLayout)rhiWriteDescriptorSetElement.pImageInfo->imageLayout;

                vkDescriptorImageInfoPtr = &vkDescriptorImageInfo;
                imageInfoCurrent++;
            }

            const VkDescriptorBufferInfo* vkDescriptorBufferInfoPtr = nullptr;
            if (rhiWriteDescriptorSetElement.pBufferInfo != nullptr)
            {
                auto& vkDescriptorBufferInfo = vkDescriptorBufferInfoList[bufferInfoCurrent];
                vkDescriptorBufferInfo.buffer = ((VulkanBuffer*)rhiWriteDescriptorSetElement.pBufferInfo->buffer)->GetResource();
                vkDescriptorBufferInfo.offset = (VkDeviceSize)rhiWriteDescriptorSetElement.pBufferInfo->offset;
                vkDescriptorBufferInfo.range = (VkDeviceSize)rhiWriteDescriptorSetElement.pBufferInfo->range;

                vkDescriptorBufferInfoPtr = &vkDescriptorBufferInfo;
                bufferInfoCurrent++;
            }

            vkWriteDescriptorSetElement.sType = (VkStructureType)rhiWriteDescriptorSetElement.sType;
            vkWriteDescriptorSetElement.pNext = (const void*)rhiWriteDescriptorSetElement.pNext;
            vkWriteDescriptorSetElement.dstSet = ((VulkanDescriptorSet*)rhiWriteDescriptorSetElement.dstSet)->GetResource();
            vkWriteDescriptorSetElement.dstBinding = rhiWriteDescriptorSetElement.dstBinding;
            vkWriteDescriptorSetElement.dstArrayElement = rhiWriteDescriptorSetElement.dstArrayElement;
            vkWriteDescriptorSetElement.descriptorCount = rhiWriteDescriptorSetElement.descriptorCount;
            vkWriteDescriptorSetElement.descriptorType = (VkDescriptorType)rhiWriteDescriptorSetElement.descriptorType;
            vkWriteDescriptorSetElement.pImageInfo = vkDescriptorImageInfoPtr;
            vkWriteDescriptorSetElement.pBufferInfo = vkDescriptorBufferInfoPtr;
            //vk_write_descriptor_set_element.pTexelBufferView = &((VulkanBufferView*)rhi_write_descriptor_set_element.pTexelBufferView)->GetResource();
        };

        if (imageInfoCurrent != imageInfoCount
            || bufferInfoCurrent != bufferInfoCount)
        {
            GAL_CORE_ERROR("[VulkanRHI] image_info_current != image_info_count || buffer_info_current != buffer_info_count");
            return;
        }

        //copy_descriptor_set
        int copyDescriptorSetSize = descriptorCopyCount;
        std::vector<VkCopyDescriptorSet> vkCopyDescriptorSetList(copyDescriptorSetSize);
        for (int i = 0; i < copyDescriptorSetSize; ++i)
        {
            const auto& rhiCopyDescriptorSetElement = pDescriptorCopies[i];
            auto& vkCopyDescriptorSetElement = vkCopyDescriptorSetList[i];

            vkCopyDescriptorSetElement.sType = (VkStructureType)rhiCopyDescriptorSetElement.sType;
            vkCopyDescriptorSetElement.pNext = (const void*)rhiCopyDescriptorSetElement.pNext;
            vkCopyDescriptorSetElement.srcSet = ((VulkanDescriptorSet*)rhiCopyDescriptorSetElement.srcSet)->GetResource();
            vkCopyDescriptorSetElement.srcBinding = rhiCopyDescriptorSetElement.srcBinding;
            vkCopyDescriptorSetElement.srcArrayElement = rhiCopyDescriptorSetElement.srcArrayElement;
            vkCopyDescriptorSetElement.dstSet = ((VulkanDescriptorSet*)rhiCopyDescriptorSetElement.dstSet)->GetResource();
            vkCopyDescriptorSetElement.dstBinding = rhiCopyDescriptorSetElement.dstBinding;
            vkCopyDescriptorSetElement.dstArrayElement = rhiCopyDescriptorSetElement.dstArrayElement;
            vkCopyDescriptorSetElement.descriptorCount = rhiCopyDescriptorSetElement.descriptorCount;
        };

        vkUpdateDescriptorSets(Device, descriptorWriteCount, vkWriteDescriptorSetList.data(), descriptorCopyCount, vkCopyDescriptorSetList.data());
    }

    bool VulkanRHI::QueueSubmit(RHIQueue* queue, uint32_t submitCount, const RHISubmitInfo* pSubmits, RHIFence* fence)
    {
        //submit_info
        int commandBufferSizeTotal = 0;
        int semaphoreSizeTotal = 0;
        int signalSemaphoreSizeTotal = 0;
        int pipelineStageFlagsSizeTotal = 0;

        int submitInfoSize = submitCount;
        for (int i = 0; i < submitInfoSize; ++i)
        {
            const auto& rhiSubmitInfoElement = pSubmits[i];
            commandBufferSizeTotal += rhiSubmitInfoElement.commandBufferCount;
            semaphoreSizeTotal += rhiSubmitInfoElement.waitSemaphoreCount;
            signalSemaphoreSizeTotal += rhiSubmitInfoElement.signalSemaphoreCount;
            pipelineStageFlagsSizeTotal += rhiSubmitInfoElement.waitSemaphoreCount;
        }
        std::vector<VkCommandBuffer> vkCommandBufferListExternal(commandBufferSizeTotal);
        std::vector<VkSemaphore> vkSemaphoreListExternal(semaphoreSizeTotal);
        std::vector<VkSemaphore> vkSignalSemaphoreListExternal(signalSemaphoreSizeTotal);
        std::vector<VkPipelineStageFlags> vkPipelineStageFlagsListExternal(pipelineStageFlagsSizeTotal);

        int commandBufferSizeCurrent = 0;
        int semaphoreSizeCurrent = 0;
        int signalSemaphoreSizeCurrent = 0;
        int pipelineStageFlagsSizeCurrent = 0;


        std::vector<VkSubmitInfo> vkSubmitInfoList(submitInfoSize);
        for (int i = 0; i < submitInfoSize; ++i)
        {
            const auto& rhiSubmitInfoElement = pSubmits[i];
            auto& vkSubmitInfoElement = vkSubmitInfoList[i];

            vkSubmitInfoElement.sType = (VkStructureType)rhiSubmitInfoElement.sType;
            vkSubmitInfoElement.pNext = (const void*)rhiSubmitInfoElement.pNext;

            //command_buffer
            if (rhiSubmitInfoElement.commandBufferCount > 0)
            {
                vkSubmitInfoElement.commandBufferCount = rhiSubmitInfoElement.commandBufferCount;
                vkSubmitInfoElement.pCommandBuffers = &vkCommandBufferListExternal[commandBufferSizeCurrent];
                int commandBufferSize = rhiSubmitInfoElement.commandBufferCount;
                for (int i = 0; i < commandBufferSize; ++i)
                {
                    const auto& rhiCommandBufferElement = rhiSubmitInfoElement.pCommandBuffers[i];
                    auto& vkCommandBufferElement = vkCommandBufferListExternal[commandBufferSizeCurrent];

                    vkCommandBufferElement = ((VulkanCommandBuffer*)rhiCommandBufferElement)->GetResource();

                    commandBufferSizeCurrent++;
                };
            }

            //semaphore
            if (rhiSubmitInfoElement.waitSemaphoreCount > 0)
            {
                vkSubmitInfoElement.waitSemaphoreCount = rhiSubmitInfoElement.waitSemaphoreCount;
                vkSubmitInfoElement.pWaitSemaphores = &vkSemaphoreListExternal[semaphoreSizeCurrent];
                int semaphoreSize = rhiSubmitInfoElement.waitSemaphoreCount;
                for (int i = 0; i < semaphoreSize; ++i)
                {
                    const auto& rhiSemaphoreElement = rhiSubmitInfoElement.pWaitSemaphores[i];
                    auto& vkSemaphoreElement = vkSemaphoreListExternal[semaphoreSizeCurrent];

                    vkSemaphoreElement = ((VulkanSemaphore*)rhiSemaphoreElement)->GetResource();

                    semaphoreSizeCurrent++;
                };
            }

            //signal_semaphore
            if (rhiSubmitInfoElement.signalSemaphoreCount > 0)
            {
                vkSubmitInfoElement.signalSemaphoreCount = rhiSubmitInfoElement.signalSemaphoreCount;
                vkSubmitInfoElement.pSignalSemaphores = &vkSignalSemaphoreListExternal[signalSemaphoreSizeCurrent];
                int signalSemaphoreSize = rhiSubmitInfoElement.signalSemaphoreCount;
                for (int i = 0; i < signalSemaphoreSize; ++i)
                {
                    const auto& rhiSignalSemaphoreElement = rhiSubmitInfoElement.pSignalSemaphores[i];
                    auto& vkSignalSemaphoreElement = vkSignalSemaphoreListExternal[signalSemaphoreSizeCurrent];

                    vkSignalSemaphoreElement = ((VulkanSemaphore*)rhiSignalSemaphoreElement)->GetResource();

                    signalSemaphoreSizeCurrent++;
                };
            }

            //pipeline_stage_flags
            if (rhiSubmitInfoElement.waitSemaphoreCount > 0)
            {
                vkSubmitInfoElement.pWaitDstStageMask = &vkPipelineStageFlagsListExternal[pipelineStageFlagsSizeCurrent];
                int pipelineStageFlagsSize = rhiSubmitInfoElement.waitSemaphoreCount;
                for (int i = 0; i < pipelineStageFlagsSize; ++i)
                {
                    const auto& rhiPipelineStageFlagsElement = rhiSubmitInfoElement.pWaitDstStageMask[i];
                    auto& vkPipelineStageFlagsElement = vkPipelineStageFlagsListExternal[pipelineStageFlagsSizeCurrent];

                    vkPipelineStageFlagsElement = (VkPipelineStageFlags)rhiPipelineStageFlagsElement;

                    pipelineStageFlagsSizeCurrent++;
                };
            }
        };


        if ((commandBufferSizeTotal != commandBufferSizeCurrent)
            || (semaphoreSizeTotal != semaphoreSizeCurrent)
            || (signalSemaphoreSizeTotal != signalSemaphoreSizeCurrent)
            || (pipelineStageFlagsSizeTotal != pipelineStageFlagsSizeCurrent))
        {
            GAL_CORE_ERROR("submit info is not right!");
            return false;
        }

        VkFence vk_fence = VK_NULL_HANDLE;
        if (fence != nullptr)
        {
            vk_fence = ((VulkanFence*)fence)->GetResource();
        }

        VkResult result = vkQueueSubmit(((VulkanQueue*)queue)->GetResource(), submitCount, vkSubmitInfoList.data(), vk_fence);
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to submit queue")
    }

    bool VulkanRHI::QueueWaitIdle(RHIQueue* queue)
    {
        VkResult result = vkQueueWaitIdle(((VulkanQueue*)queue)->GetResource());
        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to wait queue idle")
    }

    void VulkanRHI::CmdPipelineBarrier(RHICommandBuffer* commandBuffer,
                                       RHIPipelineStageFlags srcStageMask,
                                       RHIPipelineStageFlags dstStageMask,
                                       RHIDependencyFlags dependencyFlags,
                                       uint32_t memoryBarrierCount,
                                       const RHIMemoryBarrier* pMemoryBarriers,
                                       uint32_t bufferMemoryBarrierCount,
                                       const RHIBufferMemoryBarrier* pBufferMemoryBarriers,
                                       uint32_t imageMemoryBarrierCount,
                                       const RHIImageMemoryBarrier* pImageMemoryBarriers)
    {

        //memory_barrier
        int memoryBarrierSize = memoryBarrierCount;
        std::vector<VkMemoryBarrier> vkMemoryBarrierList(memoryBarrierSize);
        for (int i = 0; i < memoryBarrierSize; ++i)
        {
            const auto& rhiMemoryBarrierElement = pMemoryBarriers[i];
            auto& vkMemoryBarrierElement = vkMemoryBarrierList[i];


            vkMemoryBarrierElement.sType = (VkStructureType)rhiMemoryBarrierElement.sType;
            vkMemoryBarrierElement.pNext = (const void*)rhiMemoryBarrierElement.pNext;
            vkMemoryBarrierElement.srcAccessMask = (VkAccessFlags)rhiMemoryBarrierElement.srcAccessMask;
            vkMemoryBarrierElement.dstAccessMask = (VkAccessFlags)rhiMemoryBarrierElement.dstAccessMask;
        };

        //buffer_memory_barrier
        int bufferMemoryBarrierSize = bufferMemoryBarrierCount;
        std::vector<VkBufferMemoryBarrier> vkBufferMemoryBarrierList(bufferMemoryBarrierSize);
        for (int i = 0; i < bufferMemoryBarrierSize; ++i)
        {
            const auto& rhiBufferMemoryBarrierElement = pBufferMemoryBarriers[i];
            auto& vkBufferMemoryBarrierElement = vkBufferMemoryBarrierList[i];

            vkBufferMemoryBarrierElement.sType = (VkStructureType)rhiBufferMemoryBarrierElement.sType;
            vkBufferMemoryBarrierElement.pNext = (const void*)rhiBufferMemoryBarrierElement.pNext;
            vkBufferMemoryBarrierElement.srcAccessMask = (VkAccessFlags)rhiBufferMemoryBarrierElement.srcAccessMask;
            vkBufferMemoryBarrierElement.dstAccessMask = (VkAccessFlags)rhiBufferMemoryBarrierElement.dstAccessMask;
            vkBufferMemoryBarrierElement.srcQueueFamilyIndex = rhiBufferMemoryBarrierElement.srcQueueFamilyIndex;
            vkBufferMemoryBarrierElement.dstQueueFamilyIndex = rhiBufferMemoryBarrierElement.dstQueueFamilyIndex;
            vkBufferMemoryBarrierElement.buffer = ((VulkanBuffer*)rhiBufferMemoryBarrierElement.buffer)->GetResource();
            vkBufferMemoryBarrierElement.offset = (VkDeviceSize)rhiBufferMemoryBarrierElement.offset;
            vkBufferMemoryBarrierElement.size = (VkDeviceSize)rhiBufferMemoryBarrierElement.size;
        };

        //image_memory_barrier
        int imageMemoryBarrierSize = imageMemoryBarrierCount;
        std::vector<VkImageMemoryBarrier> vkImageMemoryBarrierList(imageMemoryBarrierSize);
        for (int i = 0; i < imageMemoryBarrierSize; ++i)
        {
            const auto& rhiImageMemoryBarrierElement = pImageMemoryBarriers[i];
            auto& vkImageMemoryBarrierElement = vkImageMemoryBarrierList[i];

            VkImageSubresourceRange imageSubresourceRange{};
            imageSubresourceRange.aspectMask = (VkImageAspectFlags)rhiImageMemoryBarrierElement.subresourceRange.aspectMask;
            imageSubresourceRange.baseMipLevel = rhiImageMemoryBarrierElement.subresourceRange.baseMipLevel;
            imageSubresourceRange.levelCount = rhiImageMemoryBarrierElement.subresourceRange.levelCount;
            imageSubresourceRange.baseArrayLayer = rhiImageMemoryBarrierElement.subresourceRange.baseArrayLayer;
            imageSubresourceRange.layerCount = rhiImageMemoryBarrierElement.subresourceRange.layerCount;

            vkImageMemoryBarrierElement.sType = (VkStructureType)rhiImageMemoryBarrierElement.sType;
            vkImageMemoryBarrierElement.pNext = (const void*)rhiImageMemoryBarrierElement.pNext;
            vkImageMemoryBarrierElement.srcAccessMask = (VkAccessFlags)rhiImageMemoryBarrierElement.srcAccessMask;
            vkImageMemoryBarrierElement.dstAccessMask = (VkAccessFlags)rhiImageMemoryBarrierElement.dstAccessMask;
            vkImageMemoryBarrierElement.oldLayout = (VkImageLayout)rhiImageMemoryBarrierElement.oldLayout;
            vkImageMemoryBarrierElement.newLayout = (VkImageLayout)rhiImageMemoryBarrierElement.newLayout;
            vkImageMemoryBarrierElement.srcQueueFamilyIndex = rhiImageMemoryBarrierElement.srcQueueFamilyIndex;
            vkImageMemoryBarrierElement.dstQueueFamilyIndex = rhiImageMemoryBarrierElement.dstQueueFamilyIndex;
            vkImageMemoryBarrierElement.image = ((VulkanImage*)rhiImageMemoryBarrierElement.image)->GetResource();
            vkImageMemoryBarrierElement.subresourceRange = imageSubresourceRange;
        };

        vkCmdPipelineBarrier(
            ((VulkanCommandBuffer*)commandBuffer)->GetResource(),
            (RHIPipelineStageFlags)srcStageMask,
            (RHIPipelineStageFlags)dstStageMask,
            (RHIDependencyFlags)dependencyFlags,
            memoryBarrierCount,
            vkMemoryBarrierList.data(),
            bufferMemoryBarrierCount,
            vkBufferMemoryBarrierList.data(),
            imageMemoryBarrierCount,
            vkImageMemoryBarrierList.data());
    }

    void VulkanRHI::CmdDraw(RHICommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        vkCmdDraw(((VulkanCommandBuffer*)commandBuffer)->GetResource(), vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanRHI::CmdDispatch(RHICommandBuffer* commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        vkCmdDispatch(((VulkanCommandBuffer*)commandBuffer)->GetResource(), groupCountX, groupCountY, groupCountZ);
    }

    void VulkanRHI::CmdDispatchIndirect(RHICommandBuffer* commandBuffer, RHIBuffer* buffer, RHIDeviceSize offset)
    {
        vkCmdDispatchIndirect(((VulkanCommandBuffer*)commandBuffer)->GetResource(), ((VulkanBuffer*)buffer)->GetResource(), offset);
    }

    void VulkanRHI::CmdCopyImageToBuffer(
        RHICommandBuffer* commandBuffer,
        RHIImage* srcImage,
        RHIImageLayout srcImageLayout,
        RHIBuffer* dstBuffer,
        uint32_t regionCount,
        const RHIBufferImageCopy* pRegions)
    {
        //buffer_image_copy
        int bufferImageCopySize = regionCount;
        std::vector<VkBufferImageCopy> vkBufferImageCopyList(bufferImageCopySize);
        for (int i = 0; i < bufferImageCopySize; ++i)
        {
            const auto& rhiBufferImageCopyElement = pRegions[i];
            auto& vkBufferImageCopyElement = vkBufferImageCopyList[i];

            VkImageSubresourceLayers imageSubresourceLayers{};
            imageSubresourceLayers.aspectMask = (VkImageAspectFlags)rhiBufferImageCopyElement.imageSubresource.aspectMask;
            imageSubresourceLayers.mipLevel = rhiBufferImageCopyElement.imageSubresource.mipLevel;
            imageSubresourceLayers.baseArrayLayer = rhiBufferImageCopyElement.imageSubresource.baseArrayLayer;
            imageSubresourceLayers.layerCount = rhiBufferImageCopyElement.imageSubresource.layerCount;

            VkOffset3D offset3d{};
            offset3d.x = rhiBufferImageCopyElement.imageOffset.x;
            offset3d.y = rhiBufferImageCopyElement.imageOffset.y;
            offset3d.z = rhiBufferImageCopyElement.imageOffset.z;

            VkExtent3D extent3d{};
            extent3d.width = rhiBufferImageCopyElement.imageExtent.width;
            extent3d.height = rhiBufferImageCopyElement.imageExtent.height;
            extent3d.depth = rhiBufferImageCopyElement.imageExtent.depth;

            VkBufferImageCopy bufferImageCopy{};
            bufferImageCopy.bufferOffset = (VkDeviceSize)rhiBufferImageCopyElement.bufferOffset;
            bufferImageCopy.bufferRowLength = rhiBufferImageCopyElement.bufferRowLength;
            bufferImageCopy.bufferImageHeight = rhiBufferImageCopyElement.bufferImageHeight;
            bufferImageCopy.imageSubresource = imageSubresourceLayers;
            bufferImageCopy.imageOffset = offset3d;
            bufferImageCopy.imageExtent = extent3d;

            vkBufferImageCopyElement.bufferOffset = (VkDeviceSize)rhiBufferImageCopyElement.bufferOffset;
            vkBufferImageCopyElement.bufferRowLength = rhiBufferImageCopyElement.bufferRowLength;
            vkBufferImageCopyElement.bufferImageHeight = rhiBufferImageCopyElement.bufferImageHeight;
            vkBufferImageCopyElement.imageSubresource = imageSubresourceLayers;
            vkBufferImageCopyElement.imageOffset = offset3d;
            vkBufferImageCopyElement.imageExtent = extent3d;
        };

        vkCmdCopyImageToBuffer(
            ((VulkanCommandBuffer*)commandBuffer)->GetResource(),
            ((VulkanImage*)srcImage)->GetResource(),
            (VkImageLayout)srcImageLayout,
            ((VulkanBuffer*)dstBuffer)->GetResource(),
            regionCount,
            vkBufferImageCopyList.data());
    }

    void VulkanRHI::CmdCopyImageToImage(RHICommandBuffer* commandBuffer, RHIImage* srcImage, RHIImageAspectFlagBits srcFlag, RHIImage* dstImage, RHIImageAspectFlagBits dstFlag, uint32_t width, uint32_t height)
    {
        VkImageCopy imageCopyRegion    = {};
        imageCopyRegion.srcSubresource = { (VkImageAspectFlags)srcFlag, 0, 0, 1 };
        imageCopyRegion.srcOffset = { 0, 0, 0 };
        imageCopyRegion.dstSubresource = { (VkImageAspectFlags)dstFlag, 0, 0, 1 };
        imageCopyRegion.dstOffset = { 0, 0, 0 };
        imageCopyRegion.extent = { width, height, 1 };

        vkCmdCopyImage(((VulkanCommandBuffer*)commandBuffer)->GetResource(),
                       ((VulkanImage*)srcImage)->GetResource(),
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       ((VulkanImage*)dstImage)->GetResource(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &imageCopyRegion);
    }

    void VulkanRHI::CmdCopyBuffer(RHICommandBuffer* commandBuffer, RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, uint32_t regionCount, RHIBufferCopy* pRegions)
    {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = pRegions->srcOffset;
        copyRegion.dstOffset = pRegions->dstOffset;
        copyRegion.size = pRegions->size;

        vkCmdCopyBuffer(((VulkanCommandBuffer*)commandBuffer)->GetResource(),
                        ((VulkanBuffer*)srcBuffer)->GetResource(),
                        ((VulkanBuffer*)dstBuffer)->GetResource(),
                        regionCount,
                        &copyRegion);
    }

    void VulkanRHI::CreateCommandBuffers()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
        commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1U;

        for (uint32_t i = 0; i < MaxFramesInFlight; ++i)
        {
            commandBufferAllocateInfo.commandPool = CommandPools[i];
            VkCommandBuffer vkCommandBuffer;
            if (vkAllocateCommandBuffers(Device, &commandBufferAllocateInfo, &vkCommandBuffer) != VK_SUCCESS)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to allocate command buffers");
            }
            VkCommandBuffers[i] = vkCommandBuffer;
            CommandBuffers[i] = new VulkanCommandBuffer();
            ((VulkanCommandBuffer*)CommandBuffers[i])->SetResource(vkCommandBuffer);
        }
    }

    void VulkanRHI::CreateDescriptorPool()
    {
        // Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
        // should be big enough, and thus we can sub-allocate DescriptorSet from
        // DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.

        VkDescriptorPoolSize poolSizes[7];
        poolSizes[0].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        poolSizes[0].descriptorCount = 3 + 2 + 2 + 2 + 1 + 1 + 3 + 3;
        poolSizes[1].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[1].descriptorCount = 1 + 1 + 1 * m_MaxVertexBlendingMeshCount;
        poolSizes[2].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = 1 * m_MaxMaterialCount;
        poolSizes[3].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[3].descriptorCount = 3 + 5 * m_MaxMaterialCount + 1 + 1; // ImGui_ImplVulkan_CreateDeviceObjects
        poolSizes[4].type            = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        poolSizes[4].descriptorCount = 4 + 1 + 1 + 2;
        poolSizes[5].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[5].descriptorCount = 3;
        poolSizes[6].type            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[6].descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(poolSizes[0]);
        poolInfo.pPoolSizes    = poolSizes;
        poolInfo.maxSets =
            1 + 1 + 1 + m_MaxMaterialCount + m_MaxVertexBlendingMeshCount + 1 + 1; // +skybox + axis descriptor set
        poolInfo.flags = 0U;

        if (vkCreateDescriptorPool(Device, &poolInfo, nullptr, &GlobalVkDescriptorPool) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to create descriptor pool!");
        }

        DescriptorPool = new VulkanDescriptorPool();
        ((VulkanDescriptorPool*)DescriptorPool)->SetResource(GlobalVkDescriptorPool);
    }

    // semaphore : signal an image is ready for rendering // ready for presentation
    // (m_vulkan_context._swapchain_images --> semaphores, fences)
    void VulkanRHI::CreateSyncPrimitives()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // the fence is initialized as signaled

        for (uint32_t i = 0; i < MaxFramesInFlight; i++)
        {
            ImageAvailableForTexturescopySemaphores[i] = new VulkanSemaphore();
            if (vkCreateSemaphore(
                    Device, &semaphoreCreateInfo, nullptr, &ImageAvailableForRenderSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(
                    Device, &semaphoreCreateInfo, nullptr, &ImageFinishedForPresentationSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(
                    Device, &semaphoreCreateInfo, nullptr, &(((VulkanSemaphore*)ImageAvailableForTexturescopySemaphores[i])->GetResource())) !=
                    VK_SUCCESS ||
                vkCreateFence(Device, &fenceCreateInfo, nullptr, &IsFrameInFlightFences[i]) != VK_SUCCESS)
            {
                GAL_CORE_ERROR("[VulkanRHI] Failed to create semaphore & fence!");
            }

            RhiIsFrameInFlightFences[i] = new VulkanFence();
            ((VulkanFence*)RhiIsFrameInFlightFences[i])->SetResource(IsFrameInFlightFences[i]);
        }
    }

    void VulkanRHI::CreateFramebufferImageAndView()
    {
        VulkanUtil::CreateImage(PhysicalDevice,
                                Device,
                                SwapchainExtent.width,
                                SwapchainExtent.height,
                                (VkFormat)DepthImageFormat,
                                VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                ((VulkanImage*)DepthImage)->GetResource(),
                                DepthImageMemory,
                                0,
                                1,
                                1);

        ((VulkanImageView*)DepthImageView)->SetResource(
            VulkanUtil::CreateImageView(Device, ((VulkanImage*)DepthImage)->GetResource(), (VkFormat)DepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1));
    }

    RHISampler* VulkanRHI::GetOrCreateDefaultSampler(RHIDefaultSamplerType type)
    {
        switch (type)
        {
            case Galaxy::Default_Sampler_Linear:
                if (m_LinearSampler == nullptr)
                {
                    m_LinearSampler = new VulkanSampler();
                    ((VulkanSampler*)m_LinearSampler)->SetResource(VulkanUtil::GetOrCreateLinearSampler(PhysicalDevice, Device));
                }
                return m_LinearSampler;
                break;

            case Galaxy::Default_Sampler_Nearest:
                if (m_NearestSampler == nullptr)
                {
                    m_NearestSampler = new VulkanSampler();
                    ((VulkanSampler*)m_NearestSampler)->SetResource(VulkanUtil::GetOrCreateNearestSampler(PhysicalDevice, Device));
                }
                return m_NearestSampler;
                break;

            default:
                return nullptr;
                break;
        }
    }

    RHISampler* VulkanRHI::GetOrCreateMipmapSampler(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
        {
            GAL_CORE_ERROR("[VulkanRHI] GetOrCreateMipmapSampler width == 0 || height == 0 !!!");
            return nullptr;
        }
        RHISampler* sampler;
        uint32_t  mipLevels = floor(log2(std::max(width, height))) + 1;
        auto      findSampler = m_MipmapSamplerMap.find(mipLevels);
        if (findSampler != m_MipmapSamplerMap.end())
        {
            return findSampler->second;
        }
        else
        {
            sampler = new VulkanSampler();

            VkSampler vkSampler = VulkanUtil::GetOrCreateMipmapSampler(PhysicalDevice, Device, width, height);

            ((VulkanSampler*)sampler)->SetResource(vkSampler);

            m_MipmapSamplerMap.insert(std::make_pair(mipLevels, sampler));

            return sampler;
        }
    }

    RHIShader* VulkanRHI::CreateShaderModule(const std::vector<unsigned char>& shaderCode)
    {
        RHIShader* shahder = new VulkanShader();

        VkShaderModule vkShader =  VulkanUtil::CreateShaderModule(Device, shaderCode);

        ((VulkanShader*)shahder)->SetResource(vkShader);

        return shahder;
    }

    void VulkanRHI::CreateBuffer(RHIDeviceSize size, RHIBufferUsageFlags usage, RHIMemoryPropertyFlags properties, RHIBuffer* & buffer, RHIDeviceMemory* & buffer_memory)
    {
        VkBuffer vkBuffer;
        VkDeviceMemory vkDeviceMemory;

        VulkanUtil::CreateBuffer(PhysicalDevice, Device, size, usage, properties, vkBuffer, vkDeviceMemory);

        buffer = new VulkanBuffer();
        buffer_memory = new VulkanDeviceMemory();
        ((VulkanBuffer*)buffer)->SetResource(vkBuffer);
        ((VulkanDeviceMemory*)buffer_memory)->SetResource(vkDeviceMemory);
    }

    void VulkanRHI::CreateBufferAndInitialize(RHIBufferUsageFlags usage, RHIMemoryPropertyFlags properties, RHIBuffer*& buffer, RHIDeviceMemory*& bufferMemory, RHIDeviceSize size, void* data, int                    dataSize)
    {
        VkBuffer vkBuffer;
        VkDeviceMemory vkDeviceMemory;

        VulkanUtil::CreateBufferAndInitialize(Device, PhysicalDevice, usage, properties, &vkBuffer, &vkDeviceMemory, size, data, dataSize);

        buffer = new VulkanBuffer();
        bufferMemory = new VulkanDeviceMemory();
        ((VulkanBuffer*)buffer)->SetResource(vkBuffer);
        ((VulkanDeviceMemory*)bufferMemory)->SetResource(vkDeviceMemory);
    }

    bool VulkanRHI::CreateBufferVma(VmaAllocator allocator, const RHIBufferCreateInfo* pBufferCreateInfo, const VmaAllocationCreateInfo* pAllocationCreateInfo, RHIBuffer* & pBuffer, VmaAllocation* pAllocation, VmaAllocationInfo* pAllocationInfo)
    {
        VkBuffer vkBuffer;
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = (VkStructureType)pBufferCreateInfo->sType;
        bufferCreateInfo.pNext = (const void*)pBufferCreateInfo->pNext;
        bufferCreateInfo.flags = (VkBufferCreateFlags)pBufferCreateInfo->flags;
        bufferCreateInfo.size = (VkDeviceSize)pBufferCreateInfo->size;
        bufferCreateInfo.usage = (VkBufferUsageFlags)pBufferCreateInfo->usage;
        bufferCreateInfo.sharingMode = (VkSharingMode)pBufferCreateInfo->sharingMode;
        bufferCreateInfo.queueFamilyIndexCount = pBufferCreateInfo->queueFamilyIndexCount;
        bufferCreateInfo.pQueueFamilyIndices = (const uint32_t*)pBufferCreateInfo->pQueueFamilyIndices;

        pBuffer = new VulkanBuffer();
        VkResult result = vmaCreateBuffer(allocator,
                                          &bufferCreateInfo,
                                          pAllocationCreateInfo,
                                          &vkBuffer,
                                          pAllocation,
                                          pAllocationInfo);

        ((VulkanBuffer*)pBuffer)->SetResource(vkBuffer);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create buffer VMA!")
    }

    bool VulkanRHI::CreateBufferWithAlignmentVma(VmaAllocator allocator, const RHIBufferCreateInfo* pBufferCreateInfo, const VmaAllocationCreateInfo* pAllocationCreateInfo, RHIDeviceSize minAlignment, RHIBuffer* &pBuffer, VmaAllocation* pAllocation, VmaAllocationInfo* pAllocationInfo)
    {
        VkBuffer vkBuffer;
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = (VkStructureType)pBufferCreateInfo->sType;
        bufferCreateInfo.pNext = (const void*)pBufferCreateInfo->pNext;
        bufferCreateInfo.flags = (VkBufferCreateFlags)pBufferCreateInfo->flags;
        bufferCreateInfo.size = (VkDeviceSize)pBufferCreateInfo->size;
        bufferCreateInfo.usage = (VkBufferUsageFlags)pBufferCreateInfo->usage;
        bufferCreateInfo.sharingMode = (VkSharingMode)pBufferCreateInfo->sharingMode;
        bufferCreateInfo.queueFamilyIndexCount = pBufferCreateInfo->queueFamilyIndexCount;
        bufferCreateInfo.pQueueFamilyIndices = (const uint32_t*)pBufferCreateInfo->pQueueFamilyIndices;

        pBuffer = new VulkanBuffer();
        VkResult result = vmaCreateBufferWithAlignment(allocator,
                                                       &bufferCreateInfo,
                                                       pAllocationCreateInfo,
                                                       minAlignment,
                                                       &vkBuffer,
                                                       pAllocation,
                                                       pAllocationInfo);

        ((VulkanBuffer*)pBuffer)->SetResource(vkBuffer);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to create buffer with alignment VMA!!!")
    }


    void VulkanRHI::CopyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, RHIDeviceSize srcOffset, RHIDeviceSize dstOffset, RHIDeviceSize size)
    {
        VkBuffer vkSrcBuffer = ((VulkanBuffer*)srcBuffer)->GetResource();
        VkBuffer vkDstBuffer = ((VulkanBuffer*)dstBuffer)->GetResource();
        VulkanUtil::CopyBuffer(this, vkSrcBuffer, vkDstBuffer, srcOffset, dstOffset, size);
    }

    void VulkanRHI::CreateImage(uint32_t image_width, uint32_t image_height, RHIFormat format, RHIImageTiling image_tiling, RHIImageUsageFlags image_usage_flags, RHIMemoryPropertyFlags memory_property_flags,
                                RHIImage* &image, RHIDeviceMemory* &memory, RHIImageCreateFlags image_create_flags, uint32_t array_layers, uint32_t miplevels)
    {
        VkImage vk_image;
        VkDeviceMemory vk_device_memory;
        VulkanUtil::CreateImage(
            PhysicalDevice,
            Device,
            image_width,
            image_height,
            (VkFormat)format,
            (VkImageTiling)image_tiling,
            (VkImageUsageFlags)image_usage_flags,
            (VkMemoryPropertyFlags)memory_property_flags,
            vk_image,
            vk_device_memory,
            (VkImageCreateFlags)image_create_flags,
            array_layers,
            miplevels);

        image = new VulkanImage();
        memory = new VulkanDeviceMemory();
        ((VulkanImage*)image)->SetResource(vk_image);
        ((VulkanDeviceMemory*)memory)->SetResource(vk_device_memory);
    }

    void VulkanRHI::CreateImageView(RHIImage* image, RHIFormat format, RHIImageAspectFlags image_aspect_flags, RHIImageViewType view_type, uint32_t layout_count, uint32_t miplevels,
                                    RHIImageView* &image_view)
    {
        image_view = new VulkanImageView();
        VkImage vk_image = ((VulkanImage*)image)->GetResource();
        VkImageView vk_image_view;
        vk_image_view = VulkanUtil::CreateImageView(Device, vk_image, (VkFormat)format, image_aspect_flags, (VkImageViewType)view_type, layout_count, miplevels);
        ((VulkanImageView*)image_view)->SetResource(vk_image_view);
    }

    void VulkanRHI::CreateGlobalImage(RHIImage* &image, RHIImageView* &image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, void* texture_image_pixels, RHIFormat texture_image_format, uint32_t miplevels)
    {
        VkImage vk_image;
        VkImageView vk_image_view;

        VulkanUtil::CreateGlobalImage(this, vk_image, vk_image_view,image_allocation,texture_image_width,texture_image_height,texture_image_pixels,texture_image_format,miplevels);

        image = new VulkanImage();
        image_view = new VulkanImageView();
        ((VulkanImage*)image)->SetResource(vk_image);
        ((VulkanImageView*)image_view)->SetResource(vk_image_view);
    }

    void VulkanRHI::CreateCubeMap(RHIImage* &image, RHIImageView* &image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, std::array<void*, 6> texture_image_pixels, RHIFormat texture_image_format, uint32_t miplevels)
    {
        VkImage vk_image;
        VkImageView vk_image_view;

        VulkanUtil::CreateCubeMap(this, vk_image, vk_image_view, image_allocation, texture_image_width, texture_image_height, texture_image_pixels, texture_image_format, miplevels);

        image = new VulkanImage();
        image_view = new VulkanImageView();
        ((VulkanImage*)image)->SetResource(vk_image);
        ((VulkanImageView*)image_view)->SetResource(vk_image_view);
    }

    void VulkanRHI::CreateSwapchainImageViews()
    {
        SwapchainImageviews.resize(SwapchainImages.size());

        // create imageview (one for each this time) for all swapchain images
        for (size_t i = 0; i < SwapchainImages.size(); i++)
        {
            VkImageView vkImageView;
            vkImageView = VulkanUtil::CreateImageView(Device,
                                                        SwapchainImages[i],
                                                        (VkFormat)SwapchainImageFormat,
                                                        VK_IMAGE_ASPECT_COLOR_BIT,
                                                        VK_IMAGE_VIEW_TYPE_2D,
                                                        1,
                                                        1);
            SwapchainImageviews[i] = new VulkanImageView();
            ((VulkanImageView*)SwapchainImageviews[i])->SetResource(vkImageView);
        }
    }

    void VulkanRHI::CreateAssetAllocator()
    {
        VmaVulkanFunctions vulkanFunctions    = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion       = m_VulkanApiVersion;
        allocatorCreateInfo.physicalDevice         = PhysicalDevice;
        allocatorCreateInfo.device                 = Device;
        allocatorCreateInfo.instance               = Instance;
        allocatorCreateInfo.pVulkanFunctions       = &vulkanFunctions;

        vmaCreateAllocator(&allocatorCreateInfo, &AssetsAllocator);
    }

    // todo : more descriptorSet
    bool VulkanRHI::AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* pAllocateInfo, RHIDescriptorSet* &pDescriptorSets)
    {
        //descriptor_set_layout
        int descriptorSetLayoutSize = pAllocateInfo->descriptorSetCount;
        std::vector<VkDescriptorSetLayout> vkDescriptorSetLayoutList(descriptorSetLayoutSize);
        for (int i = 0; i < descriptorSetLayoutSize; ++i)
        {
            const auto& rhiDescriptorSetLayoutElement = pAllocateInfo->pSetLayouts[i];
            auto& vkDescriptorSetLayoutElement = vkDescriptorSetLayoutList[i];

            vkDescriptorSetLayoutElement = ((VulkanDescriptorSetLayout*)rhiDescriptorSetLayoutElement)->GetResource();

            VulkanDescriptorSetLayout* test = ((VulkanDescriptorSetLayout*)rhiDescriptorSetLayoutElement);

            test = nullptr;
        };

        VkDescriptorSetAllocateInfo descriptorsetAllocateInfo{};
        descriptorsetAllocateInfo.sType = (VkStructureType)pAllocateInfo->sType;
        descriptorsetAllocateInfo.pNext = (const void*)pAllocateInfo->pNext;
        descriptorsetAllocateInfo.descriptorPool = ((VulkanDescriptorPool*)(pAllocateInfo->descriptorPool))->GetResource();
        descriptorsetAllocateInfo.descriptorSetCount = pAllocateInfo->descriptorSetCount;
        descriptorsetAllocateInfo.pSetLayouts = vkDescriptorSetLayoutList.data();

        VkDescriptorSet vkDescriptorSet;
        pDescriptorSets = new VulkanDescriptorSet;
        VkResult result = vkAllocateDescriptorSets(Device, &descriptorsetAllocateInfo, &vkDescriptorSet);
        ((VulkanDescriptorSet*)pDescriptorSets)->SetResource(vkDescriptorSet);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to allocate descriptor sets!")
    }

    bool VulkanRHI::AllocateCommandBuffers(const RHICommandBufferAllocateInfo* pAllocateInfo, RHICommandBuffer* &pCommandBuffers)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = (VkStructureType)pAllocateInfo->sType;
        commandBufferAllocateInfo.pNext = (const void*)pAllocateInfo->pNext;
        commandBufferAllocateInfo.commandPool = ((VulkanCommandPool*)(pAllocateInfo->commandPool))->GetResource();
        commandBufferAllocateInfo.level = (VkCommandBufferLevel)pAllocateInfo->level;
        commandBufferAllocateInfo.commandBufferCount = pAllocateInfo->commandBufferCount;

        VkCommandBuffer vkCommandBuffer;
        pCommandBuffers = new RHICommandBuffer();
        VkResult result = vkAllocateCommandBuffers(Device, &commandBufferAllocateInfo, &vkCommandBuffer);
        ((VulkanCommandBuffer*)pCommandBuffers)->SetResource(vkCommandBuffer);

        VK_CHECK_RETURN_BOOLEAN(result, "[VulkanRHI] Failed to allocate command buffers!")
    }

    void VulkanRHI::CreateSwapchain()
    {
        // query all supports of this physical device
        SwapChainSupportDetails swapchainSupportDetails = QuerySwapChainSupport(PhysicalDevice);

        // choose the best or fitting format
        VkSurfaceFormatKHR chosenSurfaceFormat =
            ChooseSwapchainSurfaceFormatFromDetails(swapchainSupportDetails.Formats);
        // choose the best or fitting present mode
        VkPresentModeKHR chosenPresentMode =
            ChooseSwapchainPresentModeFromDetails(swapchainSupportDetails.PresentModes);
        // choose the best or fitting extent
        VkExtent2D chosenExtent = ChooseSwapchainExtentFromDetails(swapchainSupportDetails.Capabilities);

        uint32_t imageCount = swapchainSupportDetails.Capabilities.minImageCount + 1;
        if (swapchainSupportDetails.Capabilities.maxImageCount > 0 &&
            imageCount > swapchainSupportDetails.Capabilities.maxImageCount)
        {
            imageCount = swapchainSupportDetails.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = Surface;

        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = chosenSurfaceFormat.format;
        createInfo.imageColorSpace  = chosenSurfaceFormat.colorSpace;
        createInfo.imageExtent      = chosenExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {QueueIndices.graphicsFamily.value(), QueueIndices.presentFamily.value()};

        if (QueueIndices.graphicsFamily != QueueIndices.presentFamily)
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = swapchainSupportDetails.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = chosenPresentMode;
        createInfo.clipped        = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(Device, &createInfo, nullptr, &Swapchain) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanRHI] Failed to create swapchain khr!!!");
        }

        vkGetSwapchainImagesKHR(Device, Swapchain, &imageCount, nullptr);
        SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(Device, Swapchain, &imageCount, SwapchainImages.data());

        SwapchainImageFormat = (RHIFormat)chosenSurfaceFormat.format;
        SwapchainExtent.height = chosenExtent.height;
        SwapchainExtent.width = chosenExtent.width;

        Scissor = {{0, 0}, {SwapchainExtent.width, SwapchainExtent.height}};
    }

    void VulkanRHI::ClearSwapchain()
    {
        for (auto imageview : SwapchainImageviews)
        {
            vkDestroyImageView(Device, ((VulkanImageView*)imageview)->GetResource(), nullptr);
        }
        vkDestroySwapchainKHR(Device, Swapchain, nullptr); // also swapchain images
    }

    void VulkanRHI::DestroyDefaultSampler(RHIDefaultSamplerType type)
    {
        switch (type)
        {
            case Galaxy::Default_Sampler_Linear:
                VulkanUtil::DestroyLinearSampler(Device);
                delete(m_LinearSampler);
                break;
            case Galaxy::Default_Sampler_Nearest:
                VulkanUtil::DestroyNearestSampler(Device);
                delete(m_NearestSampler);
                break;
            default:
                break;
        }
    }

    void VulkanRHI::DestroyMipmappedSampler()
    {
        VulkanUtil::DestroyMipmappedSampler(Device);

        for (auto sampler : m_MipmapSamplerMap)
        {
            delete sampler.second;
        }
        m_MipmapSamplerMap.clear();
    }

    void VulkanRHI::DestroyShaderModule(RHIShader* shaderModule)
    {
        vkDestroyShaderModule(Device, ((VulkanShader*)shaderModule)->GetResource(), nullptr);

        delete(shaderModule);
    }

    void VulkanRHI::DestroySemaphore(RHISemaphore* semaphore)
    {
        vkDestroySemaphore(Device, ((VulkanSemaphore*)semaphore)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroySampler(RHISampler* sampler)
    {
        vkDestroySampler(Device, ((VulkanSampler*)sampler)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyInstance(RHIInstance* instance)
    {
        vkDestroyInstance(((VulkanInstance*)instance)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyImageView(RHIImageView* imageView)
    {
        vkDestroyImageView(Device, ((VulkanImageView*)imageView)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyImage(RHIImage* image)
    {
        vkDestroyImage(Device, ((VulkanImage*)image)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyFramebuffer(RHIFramebuffer* framebuffer)
    {
        vkDestroyFramebuffer(Device, ((VulkanFramebuffer*)framebuffer)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyFence(RHIFence* fence)
    {
        vkDestroyFence(Device, ((VulkanFence*)fence)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyDevice()
    {
        vkDestroyDevice(Device, nullptr);
    }

    void VulkanRHI::DestroyCommandPool(RHICommandPool* commandPool)
    {
        vkDestroyCommandPool(Device, ((VulkanCommandPool*)commandPool)->GetResource(), nullptr);
    }

    void VulkanRHI::DestroyBuffer(RHIBuffer* &buffer)
    {
        vkDestroyBuffer(Device, ((VulkanBuffer*)buffer)->GetResource(), nullptr);
        RHI_DELETE_PTR(buffer);
    }

    void VulkanRHI::FreeCommandBuffers(RHICommandPool* commandPool, uint32_t commandBufferCount, RHICommandBuffer* pCommandBuffers)
    {
        VkCommandBuffer vk_command_buffer = ((VulkanCommandBuffer*)pCommandBuffers)->GetResource();
        vkFreeCommandBuffers(Device, ((VulkanCommandPool*)commandPool)->GetResource(), commandBufferCount, &vk_command_buffer);
    }

    void VulkanRHI::FreeMemory(RHIDeviceMemory* &memory)
    {
        vkFreeMemory(Device, ((VulkanDeviceMemory*)memory)->GetResource(), nullptr);
        RHI_DELETE_PTR(memory);
    }

    bool VulkanRHI::MapMemory(RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size, RHIMemoryMapFlags flags, void** ppData)
    {
        VkResult result = vkMapMemory(Device, ((VulkanDeviceMemory*)memory)->GetResource(), offset, size, (VkMemoryMapFlags)flags, ppData);

        if (result == VK_SUCCESS)
        {
            return true;
        }
        else
        {
            GAL_CORE_ERROR("vkMapMemory failed!");
            return false;
        }
    }

    void VulkanRHI::UnmapMemory(RHIDeviceMemory* memory)
    {
        vkUnmapMemory(Device, ((VulkanDeviceMemory*)memory)->GetResource());
    }

    void VulkanRHI::InvalidateMappedMemoryRanges(void* pNext, RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size)
    {
        VkMappedMemoryRange mappedRange{};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = ((VulkanDeviceMemory*)memory)->GetResource();
        mappedRange.offset = offset;
        mappedRange.size = size;
        vkInvalidateMappedMemoryRanges(Device, 1, &mappedRange);
    }

    void VulkanRHI::FlushMappedMemoryRanges(void* pNext, RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size)
    {
        VkMappedMemoryRange mappedRange{};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = ((VulkanDeviceMemory*)memory)->GetResource();
        mappedRange.offset = offset;
        mappedRange.size = size;
        vkFlushMappedMemoryRanges(Device, 1, &mappedRange);
    }

    RHISemaphore* &VulkanRHI::GetTextureCopySemaphore(uint32_t index)
    {
        return ImageAvailableForTexturescopySemaphores[index];
    }

    void VulkanRHI::RecreateSwapchain()
    {
        int width  = 0;
        int height = 0;
        glfwGetFramebufferSize(Window, &width, &height);
        while (width == 0 || height == 0) // minimized 0,0, pause for now
        {
            glfwGetFramebufferSize(Window, &width, &height);
            glfwWaitEvents();
        }

        VkResult resWaitForFences =
            _vkWaitForFences(Device, MaxFramesInFlight, IsFrameInFlightFences, VK_TRUE, UINT64_MAX);
        if (VK_SUCCESS != resWaitForFences)
        {
            GAL_CORE_ERROR("[VulkanRHI] _vkWaitForFences failed");
            return;
        }

        DestroyImageView(DepthImageView);
        vkDestroyImage(Device, ((VulkanImage*)DepthImage)->GetResource(), nullptr);
        vkFreeMemory(Device, DepthImageMemory, nullptr);

        for (auto imageview : SwapchainImageviews)
        {
            vkDestroyImageView(Device, ((VulkanImageView*)imageview)->GetResource(), nullptr);
        }
        vkDestroySwapchainKHR(Device, Swapchain, nullptr);

        CreateSwapchain();
        CreateSwapchainImageViews();
        CreateFramebufferImageAndView();
    }

    VkResult VulkanRHI::CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                     const VkAllocationCallbacks*              pAllocator,
                                                     VkDebugUtilsMessengerEXT*                 pDebugMessenger)
    {
        auto func =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanRHI::DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                                  VkDebugUtilsMessengerEXT     debugMessenger,
                                                  const VkAllocationCallbacks* pAllocator)
    {
        auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    QueueFamilyIndices VulkanRHI::FindQueueFamilies(VkPhysicalDevice physicalDevice) // for device and surface
    {
        QueueFamilyIndices indices;
        uint32_t           queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) // if support graphics command queue
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) // if support compute command queue
            {
                indices.computeFamily = i;
            }


            VkBool32 isPresentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,
                                                 i,
                                                 Surface,
                                                 &isPresentSupport); // if support surface presentation
            if (isPresentSupport)
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }

    bool VulkanRHI::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        for (const auto& extension : availableExtensions)
        {
            // Handle for macOS
            if (strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0)
            {
                m_DeviceExtensions.push_back("VK_KHR_portability_subset");
            }
        }

        std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());
        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VulkanRHI::IsDeviceSuitable(VkPhysicalDevice physicalDevice)
    {
        auto queueFamilyIndices           = FindQueueFamilies(physicalDevice);
        bool isExtensionsSupported = CheckDeviceExtensionSupport(physicalDevice);
        bool isSwapchainAdequate   = false;
        if (isExtensionsSupported)
        {
            SwapChainSupportDetails swapchainSupportDetails = QuerySwapChainSupport(physicalDevice);
            isSwapchainAdequate =
                !swapchainSupportDetails.Formats.empty() && !swapchainSupportDetails.PresentModes.empty();
        }

        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

        if (!queueFamilyIndices.isComplete() || !isSwapchainAdequate || !physicalDeviceFeatures.samplerAnisotropy)
        {
            return false;
        }

        return true;
    }

    Galaxy::SwapChainSupportDetails VulkanRHI::QuerySwapChainSupport(VkPhysicalDevice physicalDevice)
    {
        SwapChainSupportDetails detailsResult;

        // capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, Surface, &detailsResult.Capabilities);

        // formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, Surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            detailsResult.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                physicalDevice, Surface, &formatCount, detailsResult.Formats.data());
        }

        // present modes
        uint32_t presentmodeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, Surface, &presentmodeCount, nullptr);
        if (presentmodeCount != 0)
        {
            detailsResult.PresentModes.resize(presentmodeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice, Surface, &presentmodeCount, detailsResult.PresentModes.data());
        }

        return detailsResult;
    }

    VkFormat VulkanRHI::FindDepthFormat()
    {
        return FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    VkFormat VulkanRHI::FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                            VkImageTiling                tiling,
                                            VkFormatFeatureFlags         features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        GAL_CORE_ERROR("[VulkanRHI] Failet to findSupportedFormat");
        return VkFormat();
    }

    VkSurfaceFormatKHR
    VulkanRHI::ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
    {
        for (const auto& surfaceFormat : availableSurfaceFormats)
        {
            // TODO: select the VK_FORMAT_B8G8R8A8_SRGB surface format,
            // there is no need to do gamma correction in the fragment shader
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return surfaceFormat;
            }
        }
        return availableSurfaceFormats[0];
    }

    VkPresentModeKHR
    VulkanRHI::ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (VkPresentModeKHR presentMode : availablePresentModes)
        {
            if (VK_PRESENT_MODE_MAILBOX_KHR == presentMode)
            {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRHI::ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(Window, &width, &height);

            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width =
                std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height =
                std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanRHI::PushEvent(RHICommandBuffer* commondBuffer, const char* name, const float* color)
    {
        if (m_EnableDebugUtilsLabel)
        {
            VkDebugUtilsLabelEXT labelInfo;
            labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            labelInfo.pNext = nullptr;
            labelInfo.pLabelName = name;
            for (int i = 0; i < 4; ++i)
                labelInfo.color[i] = color[i];
            _vkCmdBeginDebugUtilsLabelEXT(((VulkanCommandBuffer*)commondBuffer)->GetResource(), &labelInfo);
        }
    }

    void VulkanRHI::PopEvent(RHICommandBuffer* commondBuffer)
    {
        if (m_EnableDebugUtilsLabel)
        {
            _vkCmdEndDebugUtilsLabelEXT(((VulkanCommandBuffer*)commondBuffer)->GetResource());
        }
    }
    bool VulkanRHI::IsPointLightShadowEnabled(){ return m_EnablePointLightShadow; }

    RHICommandBuffer* VulkanRHI::GetCurrentCommandBuffer() const
    {
        return CurrentCommandBuffer;
    }
    RHICommandBuffer* const* VulkanRHI::GetCommandBufferList() const
    {
        return CommandBuffers;
    }
    RHICommandPool* VulkanRHI::GetCommandPoor() const
    {
        return RhiCommandPool;
    }
    RHIDescriptorPool* VulkanRHI::GetDescriptorPoor() const
    {
        return DescriptorPool;
    }
    RHIFence* const* VulkanRHI::GetFenceList() const
    {
        return RhiIsFrameInFlightFences;
    }
    QueueFamilyIndices VulkanRHI::GetQueueFamilyIndices() const
    {
        return QueueIndices;
    }
    RHIQueue* VulkanRHI::GetGraphicsQueue() const
    {
        return GraphicsQueue;
    }
    RHIQueue* VulkanRHI::GetComputeQueue() const
    {
        return ComputeQueue;
    }
    RHISwapChainDesc VulkanRHI::GetSwapchainInfo()
    {
        RHISwapChainDesc desc;
        desc.imageFormat = SwapchainImageFormat;
        desc.extent = SwapchainExtent;
        desc.viewport = &Viewport;
        desc.scissor = &Scissor;
        desc.imageViews = SwapchainImageviews;
        return desc;
    }
    RHIDepthImageDesc VulkanRHI::GetDepthImageInfo() const
    {
        RHIDepthImageDesc desc;
        desc.depthImageFormat = DepthImageFormat;
        desc.depthImageView = DepthImageView;
        desc.depthImage = DepthImage;
        return desc;
    }
    uint8_t VulkanRHI::GetMaxFramesInFlight() const
    {
        return MaxFramesInFlight;
    }
    uint8_t VulkanRHI::GetCurrentFrameIndex() const
    {
        return CurrentFrameIndex;
    }
    void VulkanRHI::SetCurrentFrameIndex(uint8_t index)
    {
        CurrentFrameIndex = index;
    }
} // namespace Galaxy