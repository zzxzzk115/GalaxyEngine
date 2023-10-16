//
// RHI.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHIStruct.h"

#include <vk_mem_alloc.h>

namespace Galaxy
{
    struct RHIInitInfo
    {};

    class RHI
    {
    public:
        virtual ~RHI() = 0;

        virtual void Initialize(RHIInitInfo initInfo) = 0;
        virtual void PrepareContext()                 = 0;

        virtual bool IsPointLightShadowEnabled() = 0;
        // allocate and create
        virtual bool        AllocateCommandBuffers(const RHICommandBufferAllocateInfo* pAllocateInfo,
                                                   RHICommandBuffer*&                  pCommandBuffers)                             = 0;
        virtual bool        AllocateDescriptorSets(const RHIDescriptorSetAllocateInfo* pAllocateInfo,
                                                   RHIDescriptorSet*&                  pDescriptorSets)                             = 0;
        virtual void        CreateSwapchain()                                                                      = 0;
        virtual void        RecreateSwapchain()                                                                    = 0;
        virtual void        CreateSwapchainImageViews()                                                            = 0;
        virtual void        CreateFramebufferImageAndView()                                                        = 0;
        virtual RHISampler* GetOrCreateDefaultSampler(RHIDefaultSamplerType type)                                  = 0;
        virtual RHISampler* GetOrCreateMipmapSampler(uint32_t width, uint32_t height)                              = 0;
        virtual RHIShader*  CreateShaderModule(const std::vector<unsigned char>& shaderCode)                      = 0;
        virtual void        CreateBuffer(RHIDeviceSize          size,
                                         RHIBufferUsageFlags    usage,
                                         RHIMemoryPropertyFlags properties,
                                         RHIBuffer*&            buffer,
                                         RHIDeviceMemory*&      bufferMemory)                                          = 0;
        virtual void        CreateBufferAndInitialize(RHIBufferUsageFlags    usage,
                                                      RHIMemoryPropertyFlags properties,
                                                      RHIBuffer*&            buffer,
                                                      RHIDeviceMemory*&      bufferMemory,
                                                      RHIDeviceSize          size,
                                                      void*                  data     = nullptr,
                                                      int                    datasize = 0)                                            = 0;
        virtual bool        CreateBufferVma(VmaAllocator                   allocator,
                                            const RHIBufferCreateInfo*     pBufferCreateInfo,
                                            const VmaAllocationCreateInfo* pAllocationCreateInfo,
                                            RHIBuffer*&                    pBuffer,
                                            VmaAllocation*                 pAllocation,
                                            VmaAllocationInfo*             pAllocationInfo)                                    = 0;
        virtual bool        CreateBufferWithAlignmentVma(VmaAllocator                   allocator,
                                                         const RHIBufferCreateInfo*     pBufferCreateInfo,
                                                         const VmaAllocationCreateInfo* pAllocationCreateInfo,
                                                         RHIDeviceSize                  minAlignment,
                                                         RHIBuffer*&                    pBuffer,
                                                         VmaAllocation*                 pAllocation,
                                                         VmaAllocationInfo*             pAllocationInfo)                       = 0;
        virtual void        CopyBuffer(RHIBuffer*    srcBuffer,
                                       RHIBuffer*    dstBuffer,
                                       RHIDeviceSize srcOffset,
                                       RHIDeviceSize dstOffset,
                                       RHIDeviceSize size)                                                         = 0;
        virtual void        CreateImage(uint32_t               imageWidth,
                                        uint32_t               imageHeight,
                                        RHIFormat              format,
                                        RHIImageTiling         imageTiling,
                                        RHIImageUsageFlags     imageUsageFlags,
                                        RHIMemoryPropertyFlags memoryPropertyFlags,
                                        RHIImage*&             image,
                                        RHIDeviceMemory*&      memory,
                                        RHIImageCreateFlags    imageCreateFlags,
                                        uint32_t               arrayLayers,
                                        uint32_t               miplevels)                                                        = 0;
        virtual void        CreateImageView(RHIImage*           image,
                                            RHIFormat           format,
                                            RHIImageAspectFlags imageAspectFlags,
                                            RHIImageViewType    viewType,
                                            uint32_t            layoutCount,
                                            uint32_t            miplevels,
                                            RHIImageView*&      imageView)                                             = 0;
        virtual void        CreateGlobalImage(RHIImage*&     image,
                                              RHIImageView*& imageView,
                                              VmaAllocation& imageAllocation,
                                              uint32_t       textureImageWidth,
                                              uint32_t       textureImageHeight,
                                              void*          textureImagePixels,
                                              RHIFormat      textureImageFormat,
                                              uint32_t       miplevels = 0)                                              = 0;
        virtual void        CreateCubeMap(RHIImage*&           image,
                                          RHIImageView*&       imageView,
                                          VmaAllocation&       imageAllocation,
                                          uint32_t             textureImageWidth,
                                          uint32_t             textureImageHeight,
                                          std::array<void*, 6> textureImagePixels,
                                          RHIFormat            textureImageFormat,
                                          uint32_t             miplevels)                                                      = 0;
        virtual void        CreateCommandPool()                                                                    = 0;
        virtual bool CreateCommandPool(const RHICommandPoolCreateInfo* pCreateInfo, RHICommandPool*& pCommandPool) = 0;
        virtual bool CreateDescriptorPool(const RHIDescriptorPoolCreateInfo* pCreateInfo,
                                          RHIDescriptorPool*&                pDescriptorPool)                                     = 0;
        virtual bool CreateDescriptorSetLayout(const RHIDescriptorSetLayoutCreateInfo* pCreateInfo,
                                               RHIDescriptorSetLayout*&                pSetLayout)                                = 0;
        virtual bool CreateFence(const RHIFenceCreateInfo* pCreateInfo, RHIFence*& pFence)                         = 0;
        virtual bool CreateFramebuffer(const RHIFramebufferCreateInfo* pCreateInfo, RHIFramebuffer*& pFramebuffer) = 0;
        virtual bool CreateGraphicsPipelines(RHIPipelineCache*                    pipelineCache,
                                             uint32_t                             createInfoCount,
                                             const RHIGraphicsPipelineCreateInfo* pCreateInfos,
                                             RHIPipeline*&                        pPipelines)                                             = 0;
        virtual bool CreateComputePipelines(RHIPipelineCache*                   pipelineCache,
                                            uint32_t                            createInfoCount,
                                            const RHIComputePipelineCreateInfo* pCreateInfos,
                                            RHIPipeline*&                       pPipelines)                                              = 0;
        virtual bool CreatePipelineLayout(const RHIPipelineLayoutCreateInfo* pCreateInfo,
                                          RHIPipelineLayout*&                pPipelineLayout)                                     = 0;
        virtual bool CreateRenderPass(const RHIRenderPassCreateInfo* pCreateInfo, RHIRenderPass*& pRenderPass)     = 0;
        virtual bool CreateSampler(const RHISamplerCreateInfo* pCreateInfo, RHISampler*& pSampler)                 = 0;
        virtual bool CreateSemaphore(const RHISemaphoreCreateInfo* pCreateInfo, RHISemaphore*& pSemaphore)         = 0;

        // command and command write
        virtual bool
        WaitForFencesPfn(uint32_t fenceCount, RHIFence* const* pFence, RHIBool32 waitAll, uint64_t timeout) = 0;
        virtual bool ResetFencesPfn(uint32_t fenceCount, RHIFence* const* pFences)                          = 0;
        virtual bool ResetCommandPoolPfn(RHICommandPool* commandPool, RHICommandPoolResetFlags flags)       = 0;
        virtual bool BeginCommandBufferPfn(RHICommandBuffer*                commandBuffer,
                                           const RHICommandBufferBeginInfo* pBeginInfo)                     = 0;
        virtual bool EndCommandBufferPfn(RHICommandBuffer* commandBuffer)                                   = 0;
        virtual void CmdBeginRenderPassPfn(RHICommandBuffer*             commandBuffer,
                                           const RHIRenderPassBeginInfo* pRenderPassBegin,
                                           RHISubpassContents            contents)                                     = 0;
        virtual void CmdNextSubpassPfn(RHICommandBuffer* commandBuffer, RHISubpassContents contents)        = 0;
        virtual void CmdEndRenderPassPfn(RHICommandBuffer* commandBuffer)                                   = 0;
        virtual void CmdBindPipelinePfn(RHICommandBuffer*    commandBuffer,
                                        RHIPipelineBindPoint pipelineBindPoint,
                                        RHIPipeline*         pipeline)                                              = 0;
        virtual void CmdSetViewportPfn(RHICommandBuffer*  commandBuffer,
                                       uint32_t           firstViewport,
                                       uint32_t           viewportCount,
                                       const RHIViewport* pViewports)                                       = 0;
        virtual void CmdSetScissorPfn(RHICommandBuffer* commandBuffer,
                                      uint32_t          firstScissor,
                                      uint32_t          scissorCount,
                                      const RHIRect2D*  pScissors)                                           = 0;
        virtual void CmdBindVertexBuffersPfn(RHICommandBuffer*    commandBuffer,
                                             uint32_t             firstBinding,
                                             uint32_t             bindingCount,
                                             RHIBuffer* const*    pBuffers,
                                             const RHIDeviceSize* pOffsets)                                 = 0;
        virtual void CmdBindIndexBufferPfn(RHICommandBuffer* commandBuffer,
                                           RHIBuffer*        buffer,
                                           RHIDeviceSize     offset,
                                           RHIIndexType      indexType)                                          = 0;
        virtual void CmdBindDescriptorSetsPfn(RHICommandBuffer*              commandBuffer,
                                              RHIPipelineBindPoint           pipelineBindPoint,
                                              RHIPipelineLayout*             layout,
                                              uint32_t                       firstSet,
                                              uint32_t                       descriptorSetCount,
                                              const RHIDescriptorSet* const* pDescriptorSets,
                                              uint32_t                       dynamicOffsetCount,
                                              const uint32_t*                pDynamicOffsets)                              = 0;
        virtual void CmdDrawIndexedPfn(RHICommandBuffer* commandBuffer,
                                       uint32_t          indexCount,
                                       uint32_t          instanceCount,
                                       uint32_t          firstIndex,
                                       int32_t           vertexOffset,
                                       uint32_t          firstInstance)                                              = 0;
        virtual void CmdClearAttachmentsPfn(RHICommandBuffer*         commandBuffer,
                                            uint32_t                  attachmentCount,
                                            const RHIClearAttachment* pAttachments,
                                            uint32_t                  rectCount,
                                            const RHIClearRect*       pRects)                                     = 0;

        virtual bool BeginCommandBuffer(RHICommandBuffer*                commandBuffer,
                                        const RHICommandBufferBeginInfo* pBeginInfo)                               = 0;
        virtual void CmdCopyImageToBuffer(RHICommandBuffer*         commandBuffer,
                                          RHIImage*                 srcImage,
                                          RHIImageLayout            srcImageLayout,
                                          RHIBuffer*                dstBuffer,
                                          uint32_t                  regionCount,
                                          const RHIBufferImageCopy* pRegions)                                      = 0;
        virtual void CmdCopyImageToImage(RHICommandBuffer*      commandBuffer,
                                         RHIImage*              srcImage,
                                         RHIImageAspectFlagBits srcFlag,
                                         RHIImage*              dstImage,
                                         RHIImageAspectFlagBits dstFlag,
                                         uint32_t               width,
                                         uint32_t               height)                                                          = 0;
        virtual void CmdCopyBuffer(RHICommandBuffer* commandBuffer,
                                   RHIBuffer*        srcBuffer,
                                   RHIBuffer*        dstBuffer,
                                   uint32_t          regionCount,
                                   RHIBufferCopy*    pRegions)                                                        = 0;
        virtual void CmdDraw(RHICommandBuffer* commandBuffer,
                             uint32_t          vertexCount,
                             uint32_t          instanceCount,
                             uint32_t          firstVertex,
                             uint32_t          firstInstance)                                                               = 0;
        virtual void CmdDispatch(RHICommandBuffer* commandBuffer,
                                 uint32_t          groupCountX,
                                 uint32_t          groupCountY,
                                 uint32_t          groupCountZ)                                                             = 0;
        virtual void CmdDispatchIndirect(RHICommandBuffer* commandBuffer, RHIBuffer* buffer, RHIDeviceSize offset) = 0;
        virtual void CmdPipelineBarrier(RHICommandBuffer*             commandBuffer,
                                        RHIPipelineStageFlags         srcStageMask,
                                        RHIPipelineStageFlags         dstStageMask,
                                        RHIDependencyFlags            dependencyFlags,
                                        uint32_t                      memoryBarrierCount,
                                        const RHIMemoryBarrier*       pMemoryBarriers,
                                        uint32_t                      bufferMemoryBarrierCount,
                                        const RHIBufferMemoryBarrier* pBufferMemoryBarriers,
                                        uint32_t                      imageMemoryBarrierCount,
                                        const RHIImageMemoryBarrier*  pImageMemoryBarriers)                         = 0;
        virtual bool EndCommandBuffer(RHICommandBuffer* commandBuffer)                                             = 0;
        virtual void UpdateDescriptorSets(uint32_t                     descriptorWriteCount,
                                          const RHIWriteDescriptorSet* pDescriptorWrites,
                                          uint32_t                     descriptorCopyCount,
                                          const RHICopyDescriptorSet*  pDescriptorCopies)                           = 0;
        virtual bool
        QueueSubmit(RHIQueue* queue, uint32_t submitCount, const RHISubmitInfo* pSubmits, RHIFence* fence) = 0;
        virtual bool QueueWaitIdle(RHIQueue* queue)                                                        = 0;
        virtual void ResetCommandPool()                                                                    = 0;
        virtual void WaitForFences()                                                                       = 0;

        // query
        virtual void                     GetPhysicalDeviceProperties(RHIPhysicalDeviceProperties* pProperties) = 0;
        virtual RHICommandBuffer*        GetCurrentCommandBuffer() const                                       = 0;
        virtual RHICommandBuffer* const* GetCommandBufferList() const                                          = 0;
        virtual RHICommandPool*          GetCommandPoor() const                                                = 0;
        virtual RHIDescriptorPool*       GetDescriptorPoor() const                                             = 0;
        virtual RHIFence* const*         GetFenceList() const                                                  = 0;
        virtual QueueFamilyIndices       GetQueueFamilyIndices() const                                         = 0;
        virtual RHIQueue*                GetGraphicsQueue() const                                              = 0;
        virtual RHIQueue*                GetComputeQueue() const                                               = 0;
        virtual RHISwapChainDesc         GetSwapchainInfo()                                                    = 0;
        virtual RHIDepthImageDesc        GetDepthImageInfo() const                                             = 0;
        virtual uint8_t                  GetMaxFramesInFlight() const                                          = 0;
        virtual uint8_t                  GetCurrentFrameIndex() const                                          = 0;
        virtual void                     SetCurrentFrameIndex(uint8_t index)                                   = 0;

        // command write
        virtual RHICommandBuffer* BeginSingleTimeCommands()                                                         = 0;
        virtual void              EndSingleTimeCommands(RHICommandBuffer* commandBuffer)                           = 0;
        virtual bool              PrepareBeforePass(std::function<void()> passUpdateAfterRecreateSwapchain)         = 0;
        virtual void              SubmitRendering(std::function<void()> passUpdateAfterRecreateSwapchain)           = 0;
        virtual void              PushEvent(RHICommandBuffer* commondBuffer, const char* name, const float* color) = 0;
        virtual void              PopEvent(RHICommandBuffer* commondBuffer)                                        = 0;

        // destory
        virtual void Clear()                                               = 0;
        virtual void ClearSwapchain()                                      = 0;
        virtual void DestroyDefaultSampler(RHIDefaultSamplerType type)     = 0;
        virtual void DestroyMipmappedSampler()                             = 0;
        virtual void DestroyShaderModule(RHIShader* shader)                = 0;
        virtual void DestroySemaphore(RHISemaphore* semaphore)             = 0;
        virtual void DestroySampler(RHISampler* sampler)                   = 0;
        virtual void DestroyInstance(RHIInstance* instance)                = 0;
        virtual void DestroyImageView(RHIImageView* imageView)             = 0;
        virtual void DestroyImage(RHIImage* image)                         = 0;
        virtual void DestroyFramebuffer(RHIFramebuffer* framebuffer)       = 0;
        virtual void DestroyFence(RHIFence* fence)                         = 0;
        virtual void DestroyDevice()                                       = 0;
        virtual void DestroyCommandPool(RHICommandPool* commandPool)       = 0;
        virtual void DestroyBuffer(RHIBuffer*& buffer)                     = 0;
        virtual void FreeCommandBuffers(RHICommandPool*   commandPool,
                                        uint32_t          commandBufferCount,
                                        RHICommandBuffer* pCommandBuffers) = 0;

        // memory
        virtual void FreeMemory(RHIDeviceMemory*& memory)             = 0;
        virtual bool MapMemory(RHIDeviceMemory*  memory,
                               RHIDeviceSize     offset,
                               RHIDeviceSize     size,
                               RHIMemoryMapFlags flags,
                               void**            ppData)                         = 0;
        virtual void UnmapMemory(RHIDeviceMemory* memory)             = 0;
        virtual void InvalidateMappedMemoryRanges(void*            pNext,
                                                  RHIDeviceMemory* memory,
                                                  RHIDeviceSize    offset,
                                                  RHIDeviceSize    size) = 0;
        virtual void
        FlushMappedMemoryRanges(void* pNext, RHIDeviceMemory* memory, RHIDeviceSize offset, RHIDeviceSize size) = 0;

        // semaphores
        virtual RHISemaphore*& GetTextureCopySemaphore(uint32_t index) = 0;

    private:
    };

    inline RHI::~RHI() = default;
} // namespace Galaxy
