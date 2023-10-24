//
// VulkanRHI.h
//
// Created or modified by Kexuan Zhang on 20/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanResources.h"

#include <functional>
#include <map>
#include <vector>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Galaxy
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR>   PresentModes;
    };

    class VulkanRHI final : public RHI
    {
    public:
        // initialize
        virtual void Initialize(RHIInitInfo initInfo) override final;
        virtual void PrepareContext() override final;

        // allocate and create
        bool AllocateCommandBuffers(const RHICommandBufferAllocateInfo* pAllocateInfo, RHICommandBuffer* &pCommandBuffers) override;
        bool AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* pAllocateInfo, RHIDescriptorSet* &pDescriptorSets) override;
        void CreateSwapchain() override;
        void RecreateSwapchain() override;
        void CreateSwapchainImageViews() override;
        void CreateFramebufferImageAndView() override;
        RHISampler* GetOrCreateDefaultSampler(RHIDefaultSamplerType type) override;
        RHISampler* GetOrCreateMipmapSampler(uint32_t width, uint32_t height) override;
        RHIShader* CreateShaderModule(const std::vector<unsigned char>& shaderCode) override;
        void CreateBuffer(RHIDeviceSize size, RHIBufferUsageFlags usage, RHIMemoryPropertyFlags properties, RHIBuffer* &buffer, RHIDeviceMemory* &bufferMemory) override;
        void CreateBufferAndInitialize(RHIBufferUsageFlags usage, RHIMemoryPropertyFlags properties, RHIBuffer*& buffer, RHIDeviceMemory*& bufferMemory, RHIDeviceSize size, void* data = nullptr, int                    dataSize = 0) override;
        bool CreateBufferVma(VmaAllocator allocator,
                                    const RHIBufferCreateInfo* pBufferCreateInfo,
                                    const VmaAllocationCreateInfo* pAllocationCreateInfo,
                                    RHIBuffer* &pBuffer,
                                    VmaAllocation* pAllocation,
                                    VmaAllocationInfo* pAllocationInfo) override;
        bool CreateBufferWithAlignmentVma(
                   VmaAllocator allocator,
                   const RHIBufferCreateInfo* pBufferCreateInfo,
                   const VmaAllocationCreateInfo* pAllocationCreateInfo,
                   RHIDeviceSize minAlignment,
                   RHIBuffer* &pBuffer,
                   VmaAllocation* pAllocation,
                   VmaAllocationInfo* pAllocationInfo) override;
        void CopyBuffer(RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, RHIDeviceSize srcOffset, RHIDeviceSize dstOffset, RHIDeviceSize size) override;
        void CreateImage(uint32_t imageWidth, uint32_t imageHeight, RHIFormat format, RHIImageTiling imageTiling, RHIImageUsageFlags imageUsageFlags, RHIMemoryPropertyFlags memoryPropertyFlags,
                         RHIImage* &image, RHIDeviceMemory* &memory, RHIImageCreateFlags imageCreateFlags, uint32_t arrayLayers, uint32_t miplevels) override;
        void CreateImageView(RHIImage* image, RHIFormat format, RHIImageAspectFlags imageAspectFlags, RHIImageViewType viewType, uint32_t layoutCount, uint32_t miplevels,
                             RHIImageView* &imageView) override;
        void CreateGlobalImage(RHIImage* &image, RHIImageView* &imageView, VmaAllocation& imageAllocation, uint32_t textureImageWidth, uint32_t textureImageHeight, void* textureImagePixels, RHIFormat textureImageFormat, uint32_t miplevels = 0) override;
        void CreateCubeMap(RHIImage* &image, RHIImageView* &imageView, VmaAllocation& imageAllocation, uint32_t textureImageWidth, uint32_t textureImageHeight, std::array<void*, 6> textureImagePixels, RHIFormat textureImageFormat, uint32_t miplevels) override;
        bool CreateCommandPool(const RHICommandPoolCreateInfo* pCreateInfo, RHICommandPool* &pCommandPool) override;
        bool CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* pCreateInfo, RHIDescriptorPool* &pDescriptorPool) override;
        bool CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* pCreateInfo, RHIDescriptorSetLayout* &pSetLayout) override;
        bool CreateFence(const RHIFenceCreateInfo* pCreateInfo, RHIFence* &pFence) override;
        bool CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo, RHIFramebuffer* &pFramebuffer) override;
        bool CreateGraphicsPipelines(RHIPipelineCache* pipelineCache, uint32_t createInfoCount, const RHIGraphicsPipelineCreateInfo* pCreateInfos, RHIPipeline* &pPipelines) override;
        bool CreateComputePipelines(RHIPipelineCache* pipelineCache, uint32_t createInfoCount, const RHIComputePipelineCreateInfo* pCreateInfos, RHIPipeline*& pPipelines) override;
        bool CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* pCreateInfo, RHIPipelineLayout* &pPipelineLayout) override;
        bool CreateRenderPass(const RHIRenderPassCreateInfo* pCreateInfo, RHIRenderPass* &pRenderPass) override;
        bool CreateSampler(const RHISamplerCreateInfo* pCreateInfo, RHISampler* &pSampler) override;
        bool CreateSemaphore(const RHISemaphoreCreateInfo* pCreateInfo, RHISemaphore* &pSemaphore) override;

        // command and command write
        bool WaitForFencesPfn(uint32_t fenceCount, RHIFence* const* pFence, RHIBool32 waitAll, uint64_t timeout) override;
        bool ResetFencesPfn(uint32_t fenceCount, RHIFence* const* pFences) override;
        bool ResetCommandPoolPfn(RHICommandPool* commandPool, RHICommandPoolResetFlags flags) override;
        bool BeginCommandBufferPfn(RHICommandBuffer* commandBuffer, const RHICommandBufferBeginInfo* pBeginInfo) override;
        bool EndCommandBufferPfn(RHICommandBuffer* commandBuffer) override;
        void CmdBeginRenderPassPfn(RHICommandBuffer* commandBuffer, const RHIRenderPassBeginInfo* pRenderPassBegin, RHISubpassContents contents) override;
        void CmdNextSubpassPfn(RHICommandBuffer* commandBuffer, RHISubpassContents contents) override;
        void CmdEndRenderPassPfn(RHICommandBuffer* commandBuffer) override;
        void CmdBindPipelinePfn(RHICommandBuffer* commandBuffer, RHIPipelineBindPoint pipelineBindPoint, RHIPipeline* pipeline) override;
        void CmdSetViewportPfn(RHICommandBuffer* commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const RHIViewport* pViewports) override;
        void CmdSetScissorPfn(RHICommandBuffer* commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const RHIRect2D* pScissors) override;
        void CmdBindVertexBuffersPfn(
            RHICommandBuffer* commandBuffer,
            uint32_t firstBinding,
            uint32_t bindingCount,
            RHIBuffer* const* pBuffers,
            const RHIDeviceSize* pOffsets) override;
        void CmdBindIndexBufferPfn(RHICommandBuffer* commandBuffer, RHIBuffer* buffer, RHIDeviceSize offset, RHIIndexType indexType) override;
        void CmdBindDescriptorSetsPfn(
            RHICommandBuffer* commandBuffer,
            RHIPipelineBindPoint pipelineBindPoint,
            RHIPipelineLayout* layout,
            uint32_t firstSet,
            uint32_t descriptorSetCount,
            const RHIDescriptorSet* const* pDescriptorSets,
            uint32_t dynamicOffsetCount,
            const uint32_t* pDynamicOffsets) override;
        void CmdDrawIndexedPfn(RHICommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
        void CmdClearAttachmentsPfn(RHICommandBuffer* commandBuffer, uint32_t attachmentCount, const RHIClearAttachment* pAttachments, uint32_t rectCount, const RHIClearRect* pRects) override;

        bool BeginCommandBuffer(RHICommandBuffer* commandBuffer, const RHICommandBufferBeginInfo* pBeginInfo) override;
        void CmdCopyImageToBuffer(RHICommandBuffer* commandBuffer, RHIImage* srcImage, RHIImageLayout srcImageLayout, RHIBuffer* dstBuffer, uint32_t regionCount, const RHIBufferImageCopy* pRegions) override;
        void CmdCopyImageToImage(RHICommandBuffer* commandBuffer, RHIImage* srcImage, RHIImageAspectFlagBits srcFlag, RHIImage* dstImage, RHIImageAspectFlagBits dstFlag, uint32_t width, uint32_t height) override;
        void CmdCopyBuffer(RHICommandBuffer* commandBuffer, RHIBuffer* srcBuffer, RHIBuffer* dstBuffer, uint32_t regionCount, RHIBufferCopy* pRegions) override;
        void CmdDraw(RHICommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void CmdDispatch(RHICommandBuffer* commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
        void CmdDispatchIndirect(RHICommandBuffer* commandBuffer, RHIBuffer* buffer, RHIDeviceSize offset) override;
        void CmdPipelineBarrier(RHICommandBuffer* commandBuffer, RHIPipelineStageFlags srcStageMask, RHIPipelineStageFlags dstStageMask, RHIDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const RHIMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const RHIBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const RHIImageMemoryBarrier* pImageMemoryBarriers) override;
        bool EndCommandBuffer(RHICommandBuffer* commandBuffer) override;
        void UpdateDescriptorSets(uint32_t descriptorWriteCount, const RHIWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const RHICopyDescriptorSet* pDescriptorCopies) override;
        bool QueueSubmit(RHIQueue* queue, uint32_t submitCount, const RHISubmitInfo* pSubmits, RHIFence* fence) override;
        bool QueueWaitIdle(RHIQueue* queue) override;
        void ResetCommandPool() override;
        void WaitForFences() override;
        bool WaitForFences(uint32_t fenceCount, const RHIFence* const* pFences, RHIBool32 waitAll, uint64_t timeout);

        // query
        void GetPhysicalDeviceProperties(RHIPhysicalDeviceProperties* pProperties) override;
        RHICommandBuffer* GetCurrentCommandBuffer() const override;
        RHICommandBuffer* const* GetCommandBufferList() const override;
        RHICommandPool*          GetCommandPool() const override;
        RHIDescriptorPool*       GetDescriptorPool()const override;
        RHIFence* const* GetFenceList() const override;
        QueueFamilyIndices GetQueueFamilyIndices() const override;
        RHIQueue* GetGraphicsQueue() const override;
        RHIQueue* GetComputeQueue() const override;
        RHISwapChainDesc GetSwapchainInfo() override;
        RHIDepthImageDesc GetDepthImageInfo() const override;
        uint8_t GetMaxFramesInFlight() const override;
        uint8_t GetCurrentFrameIndex() const override;
        void SetCurrentFrameIndex(uint8_t index) override;

        // command write
        RHICommandBuffer* BeginSingleTimeCommands() override;
        void            EndSingleTimeCommands(RHICommandBuffer* commandBuffer) override;
        bool PrepareBeforePass(std::function<void()> passUpdateAfterRecreateSwapchain) override;
        void SubmitRendering(std::function<void()> passUpdateAfterRecreateSwapchain) override;
        void PushEvent(RHICommandBuffer* commondBuffer, const char* name, const float* color) override;
        void PopEvent(RHICommandBuffer* commondBuffer) override;

        // destory
        virtual ~VulkanRHI() override final;
        void Clear() override;
        void ClearSwapchain() override;
        void DestroyDefaultSampler(RHIDefaultSamplerType type) override;
        void DestroyMipmappedSampler() override;
        void DestroyShaderModule(RHIShader* shader) override;
        void DestroySemaphore(RHISemaphore* semaphore) override;
        void DestroySampler(RHISampler* sampler) override;
        void DestroyInstance(RHIInstance* instance) override;
        void DestroyImageView(RHIImageView* imageView) override;
        void DestroyImage(RHIImage* image) override;
        void DestroyFramebuffer(RHIFramebuffer* framebuffer) override;
        void DestroyFence(RHIFence* fence) override;
        void DestroyDevice() override;
        void DestroyCommandPool(RHICommandPool* commandPool) override;
        void DestroyBuffer(RHIBuffer* &buffer) override;
        void FreeCommandBuffers(RHICommandPool* commandPool, uint32_t commandBufferCount, RHICommandBuffer* pCommandBuffers) override;

        // memory
        void FreeMemory(RHIDeviceMemory* &memory) override;
        bool MapMemory(RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size, RHIMemoryMapFlags flags, void** ppData) override;
        void UnmapMemory(RHIDeviceMemory* memory) override;
        void InvalidateMappedMemoryRanges(void* pNext, RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size) override;
        void FlushMappedMemoryRanges(void* pNext, RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size) override;

        //semaphores
        RHISemaphore* &GetTextureCopySemaphore(uint32_t index) override;
    public:
        static uint8_t const MaxFramesInFlight {3};


        RHIQueue* GraphicsQueue{ nullptr };
        RHIQueue* ComputeQueue{ nullptr };

        RHIFormat SwapchainImageFormat{ RHI_FORMAT_UNDEFINED };
        std::vector<RHIImageView*> SwapchainImageviews;
        RHIExtent2D SwapchainExtent;
        RHIViewport Viewport;
        RHIRect2D Scissor;

        RHIFormat DepthImageFormat{ RHI_FORMAT_UNDEFINED };
        RHIImageView* DepthImageView = new VulkanImageView();

        RHIFence* RhiIsFrameInFlightFences[MaxFramesInFlight];

        RHIDescriptorPool* DescriptorPool = new VulkanDescriptorPool();

        RHICommandPool* RhiCommandPool;

        RHICommandBuffer* CommandBuffers[MaxFramesInFlight];
        RHICommandBuffer* CurrentCommandBuffer = new VulkanCommandBuffer();

        QueueFamilyIndices QueueIndices;

        GLFWwindow*        Window {nullptr};
        VkInstance         Instance {nullptr};
        VkSurfaceKHR       Surface {nullptr};
        VkPhysicalDevice   PhysicalDevice {nullptr};
        VkDevice           Device {nullptr};
        VkQueue            PresentQueue {nullptr};

        VkSwapchainKHR           Swapchain {nullptr};
        std::vector<VkImage>     SwapchainImages;

        RHIImage*        DepthImage = new VulkanImage();
        VkDeviceMemory DepthImageMemory {nullptr};

        std::vector<VkFramebuffer> SwapchainFramebuffers;

        // asset allocator use VMA library
        VmaAllocator AssetsAllocator;

        // function pointers
        // function pointers
        PFN_vkCmdBeginDebugUtilsLabelEXT _vkCmdBeginDebugUtilsLabelEXT;
        PFN_vkCmdEndDebugUtilsLabelEXT   _vkCmdEndDebugUtilsLabelEXT;
        PFN_vkWaitForFences         _vkWaitForFences;
        PFN_vkResetFences           _vkResetFences;
        PFN_vkResetCommandPool      _vkResetCommandPool;
        PFN_vkBeginCommandBuffer    _vkBeginCommandBuffer;
        PFN_vkEndCommandBuffer      _vkEndCommandBuffer;
        PFN_vkCmdBeginRenderPass    _vkCmdBeginRenderPass;
        PFN_vkCmdNextSubpass        _vkCmdNextSubpass;
        PFN_vkCmdEndRenderPass      _vkCmdEndRenderPass;
        PFN_vkCmdBindPipeline       _vkCmdBindPipeline;
        PFN_vkCmdSetViewport        _vkCmdSetViewport;
        PFN_vkCmdSetScissor         _vkCmdSetScissor;
        PFN_vkCmdBindVertexBuffers  _vkCmdBindVertexBuffers;
        PFN_vkCmdBindIndexBuffer    _vkCmdBindIndexBuffer;
        PFN_vkCmdBindDescriptorSets _vkCmdBindDescriptorSets;
        PFN_vkCmdDrawIndexed        _vkCmdDrawIndexed;
        PFN_vkCmdClearAttachments   _vkCmdClearAttachments;

        // global descriptor pool
        VkDescriptorPool GlobalVkDescriptorPool;

        // command pool and buffers
        uint8_t              CurrentFrameIndex {0};
        VkCommandPool        CommandPools[MaxFramesInFlight];
        VkCommandBuffer      VkCommandBuffers[MaxFramesInFlight];
        VkSemaphore          ImageAvailableForRenderSemaphores[MaxFramesInFlight];
        VkSemaphore          ImageFinishedForPresentationSemaphores[MaxFramesInFlight];
        RHISemaphore*        ImageAvailableForTexturescopySemaphores[MaxFramesInFlight];
        VkFence              IsFrameInFlightFences[MaxFramesInFlight];

        // TODO: set
        VkCommandBuffer   VkCurrentCommandBuffer;

        uint32_t CurrentSwapchainImageIndex;

    private:
        const std::vector<char const*> m_ValidationLayers {"VK_LAYER_KHRONOS_validation"};
        uint32_t                       m_VulkanApiVersion {VK_API_VERSION_1_0};

        std::vector<char const*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        // default sampler cache
        RHISampler* m_LinearSampler = nullptr;
        RHISampler* m_NearestSampler = nullptr;
        std::map<uint32_t, RHISampler*> m_MipmapSamplerMap;

    private:
        void CreateInstance();
        void InitializeDebugMessenger();
        void CreateWindowSurface();
        void InitializePhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool() override;;
        void CreateCommandBuffers();
        void CreateDescriptorPool();
        void CreateSyncPrimitives();
        void CreateAssetAllocator();

    public:
        bool IsPointLightShadowEnabled() override;

    private:
        bool m_EnableValidationLayers{ true };
        bool m_EnableDebugUtilsLabel{ true };
        bool m_EnablePointLightShadow{ true };

        // used in descriptor pool creation
        uint32_t m_MaxVertexBlendingMeshCount{ 256 };
        uint32_t m_MaxMaterialCount{ 256 };

        bool                     CheckValidationLayerSupport();
        std::vector<const char*> GetRequiredExtensions();
        void                     PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        VkResult                 CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                              const VkAllocationCallbacks*              pAllocator,
                                                              VkDebugUtilsMessengerEXT*                 pDebugMessenger);
        void                     DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                                               VkDebugUtilsMessengerEXT     debugMessenger,
                                                               const VkAllocationCallbacks* pAllocator);

        QueueFamilyIndices      FindQueueFamilies(VkPhysicalDevice physicalDevice);
        bool                    CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
        bool                    IsDeviceSuitable(VkPhysicalDevice physicalDevice);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice);

        VkFormat FindDepthFormat();
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling                tiling,
                                     VkFormatFeatureFlags         features);

        VkSurfaceFormatKHR
        ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
        VkPresentModeKHR
                   ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities);
    };
} // namespace Galaxy