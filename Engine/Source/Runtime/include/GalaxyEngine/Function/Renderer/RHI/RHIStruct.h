//
// RHIStruct.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 11:01.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RenderType.h"

#include <optional>

namespace Galaxy
{
/////////////////////////////////////////////////
#define RHI_DELETE_PTR(x) \
    delete (x); \
    (x) = nullptr;

    ////////////////////class////////////////////////
    class RHIBuffer
    {};
    class RHIBufferView
    {};
    class RHICommandBuffer
    {};
    class RHICommandPool
    {};
    class RHIDescriptorPool
    {};
    class RHIDescriptorSet
    {};
    class RHIDescriptorSetLayout
    {};
    class RHIDevice
    {};
    class RHIDeviceMemory
    {};
    class RHIEvent
    {};
    class RHIFence
    {};
    class RHIFramebuffer
    {};
    class RHIImage
    {};
    class RHIImageView
    {};
    class RHIInstance
    {};
    class RHIQueue
    {};
    class RHIPhysicalDevice
    {};
    class RHIPipeline
    {};
    class RHIPipelineCache
    {};
    class RHIPipelineLayout
    {};
    class RHIRenderPass
    {};
    class RHISampler
    {};
    class RHISemaphore
    {};
    class RHIShader
    {};

    ////////////////////struct//////////////////////////
    struct RHIMemoryBarrier;
    struct RHICopyDescriptorSet;
    struct RHIDescriptorImageInfo;
    struct RHIDescriptorBufferInfo;
    struct RHIOffset2D;
    struct RHISpecializationMapEntry;
    struct RHIBufferMemoryBarrier;
    struct RHIImageSubresourceRange;
    struct RHIImageMemoryBarrier;
    struct RHIExtent2D;
    struct RHIExtent3D;
    struct RHIApplicationInfo;
    struct RHIAttachmentDescription;
    struct RHIBufferCopy;
    struct RHIBufferCreateInfo;
    struct RHIBufferImageCopy;
    struct RHICommandBufferAllocateInfo;
    struct RHICommandBufferBeginInfo;
    struct RHICommandBufferInheritanceInfo;
    struct RHICommandPoolCreateInfo;
    struct RHIDescriptorPoolSize;
    struct RHIDescriptorPoolCreateInfo;
    struct RHIDescriptorSetAllocateInfo;
    struct RHIDescriptorSetLayoutBinding;
    struct RHIDescriptorSetLayoutCreateInfo;
    struct RHIDeviceCreateInfo;
    struct RHIDeviceQueueCreateInfo;
    struct RHIExtensionProperties;
    struct RHIFenceCreateInfo;
    struct RHIFormatProperties;
    struct RHIFramebufferCreateInfo;
    struct RHIGraphicsPipelineCreateInfo;
    struct RHIComputePipelineCreateInfo;
    struct RHIImageBlit;
    struct RHIImageCreateInfo;
    struct RHIImageFormatProperties;
    struct RHIImageViewCreateInfo;
    struct RHIInstanceCreateInfo;
    struct RHILayerProperties;
    struct RHIMemoryAllocateInfo;
    struct RHIMemoryHeap;
    struct RHIMemoryRequirements;
    struct RHIMemoryType;
    struct RHIPhysicalDeviceFeatures;
    struct RHIPhysicalDeviceLimits;
    struct RHIPhysicalDeviceMemoryProperties;
    struct RHIPhysicalDeviceProperties;
    struct RHIPhysicalDeviceSparseProperties;
    struct RHIPipelineColorBlendStateCreateInfo;
    struct RHIPipelineDepthStencilStateCreateInfo;
    struct RHIPipelineDynamicStateCreateInfo;
    struct RHIPipelineInputAssemblyStateCreateInfo;
    struct RHIPipelineLayoutCreateInfo;
    struct RHIPipelineMultisampleStateCreateInfo;
    struct RHIPipelineRasterizationStateCreateInfo;
    struct RHIPipelineShaderStageCreateInfo;
    struct RHIPipelineTessellationStateCreateInfo;
    struct RHIPipelineVertexInputStateCreateInfo;
    struct RHIPipelineViewportStateCreateInfo;
    struct RHIPushConstantRange;
    struct RHIQueueFamilyProperties;
    struct RHIRenderPassCreateInfo;
    struct RHISamplerCreateInfo;
    struct RHISemaphoreCreateInfo;
    struct RHIShaderModuleCreateInfo;
    struct RHISubmitInfo;
    struct RHISubpassDependency;
    struct RHISubpassDescription;
    struct RHIWriteDescriptorSet;
    struct RHIOffset3D;
    struct RHIAttachmentReference;
    struct RHIComponentMapping;
    struct RHIImageSubresourceLayers;
    struct RHIPipelineColorBlendAttachmentState;
    struct RHIRect2D;
    struct RHISpecializationInfo;
    struct RHIStencilOpState;
    struct RHIVertexInputAttributeDescription;
    struct RHIVertexInputBindingDescription;
    struct RHIViewport;
    struct RHIRenderPassBeginInfo;
    union RHIClearValue;
    union RHIClearColorValue;
    struct RHIClearDepthStencilValue;

    ////////////////////struct declaration////////////////////////
    struct RHIMemoryBarrier
    {
        RHIStructureType SType;
        const void*      PNext;
        RHIAccessFlags   SrcAccessMask;
        RHIAccessFlags   DstAccessMask;
    };

    struct RHICopyDescriptorSet
    {
        RHIStructureType  SType;
        const void*       PNext;
        RHIDescriptorSet* SrcSet;
        uint32_t          SrcBinding;
        uint32_t          SrcArrayElement;
        RHIDescriptorSet* DstSet;
        uint32_t          DstBinding;
        uint32_t          DstArrayElement;
        uint32_t          DescriptorCount;
    };

    struct RHIDescriptorImageInfo
    {
        RHISampler*    Sampler;
        RHIImageView*  ImageView;
        RHIImageLayout ImageLayout;
    };

    struct RHIDescriptorBufferInfo
    {
        RHIBuffer*    Buffer;
        RHIDeviceSize Offset;
        RHIDeviceSize Range;
    };

    struct RHIOffset2D
    {
        int32_t X;
        int32_t Y;
    };

    struct RHISpecializationMapEntry
    {
        uint32_t ConstantId;
        uint32_t Offset;
        size_t   Size;
    };

    struct RHIBufferMemoryBarrier
    {
        RHIStructureType SType;
        const void*      PNext;
        RHIAccessFlags   SrcAccessMask;
        RHIAccessFlags   DstAccessMask;
        uint32_t         SrcQueueFamilyIndex;
        uint32_t         DstQueueFamilyIndex;
        RHIBuffer*       Buffer;
        RHIDeviceSize    Offset;
        RHIDeviceSize    Size;
    };

    struct RHIImageSubresourceRange
    {
        RHIImageAspectFlags AspectMask;
        uint32_t            BaseMipLevel;
        uint32_t            LevelCount;
        uint32_t            BaseArrayLayer;
        uint32_t            LayerCount;
    };

    struct RHIImageMemoryBarrier
    {
        RHIStructureType         SType;
        const void*              PNext;
        RHIAccessFlags           SrcAccessMask;
        RHIAccessFlags           DstAccessMask;
        RHIImageLayout           OldLayout;
        RHIImageLayout           NewLayout;
        uint32_t                 SrcQueueFamilyIndex;
        uint32_t                 DstQueueFamilyIndex;
        RHIImage*                Image;
        RHIImageSubresourceRange SubresourceRange;
    };

    struct RHIExtent2D
    {
        uint32_t Width;
        uint32_t Height;
    };

    struct RHIExtent3D
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Depth;
    };

    struct RHIApplicationInfo
    {
        RHIStructureType SType;
        const void*      PNext;
        const char*      PApplicationName;
        uint32_t         ApplicationVersion;
        const char*      PEngineName;
        uint32_t         EngineVersion;
        uint32_t         ApiVersion;
    };

    struct RHIAttachmentDescription
    {
        RHIAttachmentDescriptionFlags Flags;
        RHIFormat                     Format;
        RHISampleCountFlagBits        Samples;
        RHIAttachmentLoadOp           LoadOp;
        RHIAttachmentStoreOp          StoreOp;
        RHIAttachmentLoadOp           StencilLoadOp;
        RHIAttachmentStoreOp          StencilStoreOp;
        RHIImageLayout                InitialLayout;
        RHIImageLayout                FinalLayout;
    };

    struct RHIBufferCopy
    {
        RHIDeviceSize SrcOffset;
        RHIDeviceSize DstOffset;
        RHIDeviceSize Size;
    };

    struct RHIBufferCreateInfo
    {
        RHIStructureType     SType;
        const void*          PNext;
        RHIBufferCreateFlags Flags;
        RHIDeviceSize        Size;
        RHIBufferUsageFlags  Usage;
        RHISharingMode       SharingMode;
        uint32_t             QueueFamilyIndexCount;
        const uint32_t*      PQueueFamilyIndices;
    };

    struct RHIOffset3D
    {
        int32_t X;
        int32_t Y;
        int32_t Z;
    };

    struct RHIImageSubresourceLayers
    {
        RHIImageAspectFlags AspectMask;
        uint32_t            MipLevel;
        uint32_t            BaseArrayLayer;
        uint32_t            LayerCount;
    };

    struct RHIBufferImageCopy
    {
        RHIDeviceSize             BufferOffset;
        uint32_t                  BufferRowLength;
        uint32_t                  BufferImageHeight;
        RHIImageSubresourceLayers ImageSubresource;
        RHIOffset3D               ImageOffset;
        RHIExtent3D               ImageExtent;
    };

    struct RHICommandBufferAllocateInfo
    {
        RHIStructureType      SType;
        const void*           PNext;
        RHICommandPool*       CommandPool;
        RHICommandBufferLevel Level;
        uint32_t              CommandBufferCount;
    };

    struct RHICommandBufferBeginInfo
    {
        RHIStructureType                       SType;
        const void*                            PNext;
        RHICommandBufferUsageFlags             Flags;
        const RHICommandBufferInheritanceInfo* PInheritanceInfo;
    };

    struct RHICommandBufferInheritanceInfo
    {
        RHIStructureType               SType;
        const void*                    PNext;
        RHIRenderPass*                 RenderPass;
        uint32_t                       Subpass;
        RHIFramebuffer*                Framebuffer;
        RHIBool32                      OcclusionQueryEnable;
        RHIQueryControlFlags           QueryFlags;
        RHIQueryPipelineStatisticFlags PipelineStatistics;
    };

    struct RHICommandPoolCreateInfo
    {
        RHIStructureType          SType;
        const void*               PNext;
        RHICommandPoolCreateFlags Flags;
        uint32_t                  QueueFamilyIndex;
    };

    struct RHIDescriptorPoolSize
    {
        RHIDescriptorType Type;
        uint32_t          DescriptorCount;
    };

    struct RHIDescriptorPoolCreateInfo
    {
        RHIStructureType             SType;
        const void*                  PNext;
        RHIDescriptorPoolCreateFlags Flags;
        uint32_t                     MaxSets;
        uint32_t                     PoolSizeCount;
        const RHIDescriptorPoolSize* PPoolSizes;
    };

    struct RHIDescriptorSetAllocateInfo
    {
        RHIStructureType                     SType;
        const void*                          PNext;
        RHIDescriptorPool*                   DescriptorPool;
        uint32_t                             DescriptorSetCount;
        const RHIDescriptorSetLayout* const* PSetLayouts;
    };

    struct RHIDescriptorSetLayoutBinding
    {
        uint32_t            Binding;
        RHIDescriptorType   DescriptorType;
        uint32_t            DescriptorCount;
        RHIShaderStageFlags StageFlags;
        RHISampler* const*  PImmutableSamplers = nullptr;
    };

    struct RHIDescriptorSetLayoutCreateInfo
    {
        RHIStructureType                     SType;
        const void*                          PNext;
        RHIDescriptorSetLayoutCreateFlags    Flags;
        uint32_t                             BindingCount;
        const RHIDescriptorSetLayoutBinding* PBindings;
    };

    struct RHIDeviceCreateInfo
    {
        RHIStructureType                  SType;
        const void*                       PNext;
        RHIDeviceCreateFlags              Flags;
        uint32_t                          QueueCreateInfoCount;
        const RHIDeviceQueueCreateInfo**  PQueueCreateInfos;
        uint32_t                          EnabledLayerCount;
        const char* const*                PpEnabledLayerNames;
        uint32_t                          EnabledExtensionCount;
        const char* const*                PpEnabledExtensionNames;
        const RHIPhysicalDeviceFeatures** PEnabledFeatures;
    };

    struct RHIDeviceQueueCreateInfo
    {
        RHIStructureType          SType;
        const void*               PNext;
        RHIDeviceQueueCreateFlags Flags;
        uint32_t                  QueueFamilyIndex;
        uint32_t                  QueueCount;
        const float*              PQueuePriorities;
    };

    struct RHIExtensionProperties
    {
        char     ExtensionName[RHI_MAX_EXTENSION_NAME_SIZE];
        uint32_t SpecVersion;
    };

    struct RHIFenceCreateInfo
    {
        RHIStructureType    SType;
        const void*         PNext;
        RHIFenceCreateFlags Flags;
    };

    struct RHIFormatProperties
    {
        RHIFormatFeatureFlags LinearTilingFeatures;
        RHIFormatFeatureFlags OptimalTilingFeatures;
        RHIFormatFeatureFlags BufferFeatures;
    };

    struct RHIFramebufferCreateInfo
    {
        RHIStructureType          SType;
        const void*               PNext;
        RHIFramebufferCreateFlags Flags;
        RHIRenderPass*            RenderPass;
        uint32_t                  AttachmentCount;
        RHIImageView* const*      PAttachments;
        uint32_t                  Width;
        uint32_t                  Height;
        uint32_t                  Layers;
    };

    struct RHIGraphicsPipelineCreateInfo
    {
        RHIStructureType                               SType;
        const void*                                    PNext;
        RHIPipelineCreateFlags                         Flags;
        uint32_t                                       StageCount;
        const RHIPipelineShaderStageCreateInfo*        PStages;
        const RHIPipelineVertexInputStateCreateInfo*   PVertexInputState;
        const RHIPipelineInputAssemblyStateCreateInfo* PInputAssemblyState;
        const RHIPipelineTessellationStateCreateInfo*  PTessellationState;
        const RHIPipelineViewportStateCreateInfo*      PViewportState;
        const RHIPipelineRasterizationStateCreateInfo* PRasterizationState;
        const RHIPipelineMultisampleStateCreateInfo*   PMultisampleState;
        const RHIPipelineDepthStencilStateCreateInfo*  PDepthStencilState;
        const RHIPipelineColorBlendStateCreateInfo*    PColorBlendState;
        const RHIPipelineDynamicStateCreateInfo*       PDynamicState;
        RHIPipelineLayout*                             Layout;
        RHIRenderPass*                                 RenderPass;
        uint32_t                                       Subpass;
        RHIPipeline*                                   BasePipelineHandle;
        int32_t                                        BasePipelineIndex;
    };

    struct RHIComputePipelineCreateInfo
    {
        RHIStructureType                  SType;
        const void*                       PNext;
        RHIPipelineCreateFlags            Flags;
        RHIPipelineShaderStageCreateInfo* PStages;
        RHIPipelineLayout*                Layout;
        RHIPipeline*                      BasePipelineHandle;
        int32_t                           BasePipelineIndex;
    };

    struct RHIImageBlit
    {
        RHIImageSubresourceLayers SrcSubresource;
        RHIOffset3D               SrcOffsets[2];
        RHIImageSubresourceLayers DstSubresource;
        RHIOffset3D               DstOffsets[2];
    };

    struct RHIImageCreateInfo
    {
        RHIStructureType       SType;
        const void*            PNext;
        RHIImageCreateFlags    Flags;
        RHIImageType           ImageType;
        RHIFormat              Format;
        RHIExtent3D            Extent;
        uint32_t               MipLevels;
        uint32_t               ArrayLayers;
        RHISampleCountFlagBits Samples;
        RHIImageTiling         Tiling;
        RHIImageUsageFlags     Usage;
        RHISharingMode         SharingMode;
        uint32_t               QueueFamilyIndexCount;
        const uint32_t*        PQueueFamilyIndices;
        RHIImageLayout         InitialLayout;
    };

    struct RHIImageFormatProperties
    {
        RHIExtent3D         MaxExtent;
        uint32_t            MaxMipLevels;
        uint32_t            MaxArrayLayers;
        RHISampleCountFlags SampleCounts;
        RHIDeviceSize       MaxResourceSize;
    };

    struct RHIComponentMapping
    {
        RHIComponentSwizzle R;
        RHIComponentSwizzle G;
        RHIComponentSwizzle B;
        RHIComponentSwizzle A;
    };

    struct RHIImageViewCreateInfo
    {
        RHIStructureType         SType;
        const void*              PNext;
        RHIImageViewCreateFlags  Flags;
        RHIImage*                Image;
        RHIImageViewType         ViewType;
        RHIFormat                Format;
        RHIComponentMapping      Components;
        RHIImageSubresourceRange SubresourceRange;
    };

    struct RHIInstanceCreateInfo
    {
        RHIStructureType           SType;
        const void*                PNext;
        RHIInstanceCreateFlags     Flags;
        const RHIApplicationInfo** PApplicationInfo;
        uint32_t                   EnabledLayerCount;
        const char* const*         PpEnabledLayerNames;
        uint32_t                   EnabledExtensionCount;
        const char* const*         PpEnabledExtensionNames;
    };

    struct RHILayerProperties
    {
        char     LayerName[RHI_MAX_EXTENSION_NAME_SIZE];
        uint32_t SpecVersion;
        uint32_t ImplementationVersion;
        char     Description[RHI_MAX_DESCRIPTION_SIZE];
    };

    struct RHIMemoryAllocateInfo
    {
        RHIStructureType SType;
        const void*      PNext;
        RHIDeviceSize    AllocationSize;
        uint32_t         MemoryTypeIndex;
    };

    struct RHIMemoryHeap
    {
        RHIDeviceSize      Size;
        RHIMemoryHeapFlags Flags;
    };

    struct RHIMemoryRequirements
    {
        RHIDeviceSize Size;
        RHIDeviceSize Alignment;
        uint32_t      MemoryTypeBits;
    };

    struct RHIMemoryType
    {
        RHIMemoryPropertyFlags PropertyFlags;
        uint32_t               HeapIndex;
    };

    struct RHIPhysicalDeviceFeatures
    {
        RHIBool32 RobustBufferAccess;
        RHIBool32 FullDrawIndexUint32;
        RHIBool32 ImageCubeArray;
        RHIBool32 IndependentBlend;
        RHIBool32 GeometryShader;
        RHIBool32 TessellationShader;
        RHIBool32 SampleRateShading;
        RHIBool32 DualSrcBlend;
        RHIBool32 LogicOp;
        RHIBool32 MultiDrawIndirect;
        RHIBool32 DrawIndirectFirstInstance;
        RHIBool32 DepthClamp;
        RHIBool32 DepthBiasClamp;
        RHIBool32 FillModeNonSolid;
        RHIBool32 DepthBounds;
        RHIBool32 WideLines;
        RHIBool32 LargePoints;
        RHIBool32 AlphaToOne;
        RHIBool32 MultiViewport;
        RHIBool32 SamplerAnisotropy;
        RHIBool32 TextureCompressionEtC2;
        RHIBool32 TextureCompressionAstcLdr;
        RHIBool32 TextureCompressionBc;
        RHIBool32 OcclusionQueryPrecise;
        RHIBool32 PipelineStatisticsQuery;
        RHIBool32 VertexPipelineStoresAndAtomics;
        RHIBool32 FragmentStoresAndAtomics;
        RHIBool32 ShaderTessellationAndGeometryPointSize;
        RHIBool32 ShaderImageGatherExtended;
        RHIBool32 ShaderStorageImageExtendedFormats;
        RHIBool32 ShaderStorageImageMultisample;
        RHIBool32 ShaderStorageImageReadWithoutFormat;
        RHIBool32 ShaderStorageImageWriteWithoutFormat;
        RHIBool32 ShaderUniformBufferArrayDynamicIndexing;
        RHIBool32 ShaderSampledImageArrayDynamicIndexing;
        RHIBool32 ShaderStorageBufferArrayDynamicIndexing;
        RHIBool32 ShaderStorageImageArrayDynamicIndexing;
        RHIBool32 ShaderClipDistance;
        RHIBool32 ShaderCullDistance;
        RHIBool32 ShaderFloat64;
        RHIBool32 ShaderInt64;
        RHIBool32 ShaderInt16;
        RHIBool32 ShaderResourceResidency;
        RHIBool32 ShaderResourceMinLod;
        RHIBool32 SparseBinding;
        RHIBool32 SparseResidencyBuffer;
        RHIBool32 SparseResidencyImage2D;
        RHIBool32 SparseResidencyImage3D;
        RHIBool32 SparseResidency2Samples;
        RHIBool32 SparseResidency4Samples;
        RHIBool32 SparseResidency8Samples;
        RHIBool32 SparseResidency16Samples;
        RHIBool32 SparseResidencyAliased;
        RHIBool32 VariableMultisampleRate;
        RHIBool32 InheritedQueries;
    };

    struct RHIPhysicalDeviceLimits
    {
        uint32_t            MaxImageDimension1D;
        uint32_t            MaxImageDimension2D;
        uint32_t            MaxImageDimension3D;
        uint32_t            MaxImageDimensionCube;
        uint32_t            MaxImageArrayLayers;
        uint32_t            MaxTexelBufferElements;
        uint32_t            MaxUniformBufferRange;
        uint32_t            MaxStorageBufferRange;
        uint32_t            MaxPushConstantsSize;
        uint32_t            MaxMemoryAllocationCount;
        uint32_t            MaxSamplerAllocationCount;
        RHIDeviceSize       BufferImageGranularity;
        RHIDeviceSize       SparseAddressSpaceSize;
        uint32_t            MaxBoundDescriptorSets;
        uint32_t            MaxPerStageDescriptorSamplers;
        uint32_t            MaxPerStageDescriptorUniformBuffers;
        uint32_t            MaxPerStageDescriptorStorageBuffers;
        uint32_t            MaxPerStageDescriptorSampledImages;
        uint32_t            MaxPerStageDescriptorStorageImages;
        uint32_t            MaxPerStageDescriptorInputAttachments;
        uint32_t            MaxPerStageResources;
        uint32_t            MaxDescriptorSetSamplers;
        uint32_t            MaxDescriptorSetUniformBuffers;
        uint32_t            MaxDescriptorSetUniformBuffersDynamic;
        uint32_t            MaxDescriptorSetStorageBuffers;
        uint32_t            MaxDescriptorSetStorageBuffersDynamic;
        uint32_t            MaxDescriptorSetSampledImages;
        uint32_t            MaxDescriptorSetStorageImages;
        uint32_t            MaxDescriptorSetInputAttachments;
        uint32_t            MaxVertexInputAttributes;
        uint32_t            MaxVertexInputBindings;
        uint32_t            MaxVertexInputAttributeOffset;
        uint32_t            MaxVertexInputBindingStride;
        uint32_t            MaxVertexOutputComponents;
        uint32_t            MaxTessellationGenerationLevel;
        uint32_t            MaxTessellationPatchSize;
        uint32_t            MaxTessellationControlPerVertexInputComponents;
        uint32_t            MaxTessellationControlPerVertexOutputComponents;
        uint32_t            MaxTessellationControlPerPatchOutputComponents;
        uint32_t            MaxTessellationControlTotalOutputComponents;
        uint32_t            MaxTessellationEvaluationInputComponents;
        uint32_t            MaxTessellationEvaluationOutputComponents;
        uint32_t            MaxGeometryShaderInvocations;
        uint32_t            MaxGeometryInputComponents;
        uint32_t            MaxGeometryOutputComponents;
        uint32_t            MaxGeometryOutputVertices;
        uint32_t            MaxGeometryTotalOutputComponents;
        uint32_t            MaxFragmentInputComponents;
        uint32_t            MaxFragmentOutputAttachments;
        uint32_t            MaxFragmentDualSrcAttachments;
        uint32_t            MaxFragmentCombinedOutputResources;
        uint32_t            MaxComputeSharedMemorySize;
        uint32_t            MaxComputeWorkGroupCount[3];
        uint32_t            MaxComputeWorkGroupInvocations;
        uint32_t            MaxComputeWorkGroupSize[3];
        uint32_t            SubPixelPrecisionBits;
        uint32_t            SubTexelPrecisionBits;
        uint32_t            MipmapPrecisionBits;
        uint32_t            MaxDrawIndexedIndexValue;
        uint32_t            MaxDrawIndirectCount;
        float               MaxSamplerLodBias;
        float               MaxSamplerAnisotropy;
        uint32_t            MaxViewports;
        uint32_t            MaxViewportDimensions[2];
        float               ViewportBoundsRange[2];
        uint32_t            ViewportSubPixelBits;
        size_t              MinMemoryMapAlignment;
        RHIDeviceSize       MinTexelBufferOffsetAlignment;
        RHIDeviceSize       MinUniformBufferOffsetAlignment;
        RHIDeviceSize       MinStorageBufferOffsetAlignment;
        int32_t             MinTexelOffset;
        uint32_t            MaxTexelOffset;
        int32_t             MinTexelGatherOffset;
        uint32_t            MaxTexelGatherOffset;
        float               MinInterpolationOffset;
        float               MaxInterpolationOffset;
        uint32_t            SubPixelInterpolationOffsetBits;
        uint32_t            MaxFramebufferWidth;
        uint32_t            MaxFramebufferHeight;
        uint32_t            MaxFramebufferLayers;
        RHISampleCountFlags FramebufferColorSampleCounts;
        RHISampleCountFlags FramebufferDepthSampleCounts;
        RHISampleCountFlags FramebufferStencilSampleCounts;
        RHISampleCountFlags FramebufferNoAttachmentsSampleCounts;
        uint32_t            MaxColorAttachments;
        RHISampleCountFlags SampledImageColorSampleCounts;
        RHISampleCountFlags SampledImageIntegerSampleCounts;
        RHISampleCountFlags SampledImageDepthSampleCounts;
        RHISampleCountFlags SampledImageStencilSampleCounts;
        RHISampleCountFlags StorageImageSampleCounts;
        uint32_t            MaxSampleMaskWords;
        RHIBool32           TimestampComputeAndGraphics;
        float               TimestampPeriod;
        uint32_t            MaxClipDistances;
        uint32_t            MaxCullDistances;
        uint32_t            MaxCombinedClipAndCullDistances;
        uint32_t            DiscreteQueuePriorities;
        float               PointSizeRange[2];
        float               LineWidthRange[2];
        float               PointSizeGranularity;
        float               LineWidthGranularity;
        RHIBool32           StrictLines;
        RHIBool32           StandardSampleLocations;
        RHIDeviceSize       OptimalBufferCopyOffsetAlignment;
        RHIDeviceSize       OptimalBufferCopyRowPitchAlignment;
        RHIDeviceSize       NonCoherentAtomSize;
    };

    struct RHIPhysicalDeviceMemoryProperties
    {
        uint32_t      MemoryTypeCount;
        RHIMemoryType MemoryTypes[RHI_MAX_MEMORY_TYPES];
        uint32_t      MemoryHeapCount;
        RHIMemoryHeap MemoryHeaps[RHI_MAX_MEMORY_HEAPS];
    };

    struct RHIPhysicalDeviceSparseProperties
    {
        RHIBool32 ResidencyStandard2DBlockShape;
        RHIBool32 ResidencyStandard2DMultisampleBlockShape;
        RHIBool32 ResidencyStandard3DBlockShape;
        RHIBool32 ResidencyAlignedMipSize;
        RHIBool32 ResidencyNonResidentStrict;
    };

    struct RHIPhysicalDeviceProperties
    {
        uint32_t                          ApiVersion;
        uint32_t                          DriverVersion;
        uint32_t                          VendorId;
        uint32_t                          DeviceId;
        RHIPhysicalDeviceType             DeviceType;
        char                              DeviceName[RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE];
        uint8_t                           PipelineCacheUuid[RHI_UUID_SIZE];
        RHIPhysicalDeviceLimits           Limits;
        RHIPhysicalDeviceSparseProperties SparseProperties;
    };

    struct RHIPipelineColorBlendStateCreateInfo
    {
        RHIStructureType                            SType;
        const void*                                 PNext;
        RHIPipelineColorBlendStateCreateFlags       Flags;
        RHIBool32                                   LogicOpEnable;
        RHILogicOp                                  LogicOp;
        uint32_t                                    AttachmentCount;
        const RHIPipelineColorBlendAttachmentState* PAttachments;
        float                                       BlendConstants[4];
    };

    struct RHIStencilOpState
    {
        RHIStencilOp FailOp;
        RHIStencilOp PassOp;
        RHIStencilOp DepthFailOp;
        RHICompareOp CompareOp;
        uint32_t     CompareMask;
        uint32_t     WriteMask;
        uint32_t     Reference;
    };

    struct RHIPipelineDepthStencilStateCreateInfo
    {
        RHIStructureType                        SType;
        const void*                             PNext;
        RHIPipelineDepthStencilStateCreateFlags Flags;
        RHIBool32                               DepthTestEnable;
        RHIBool32                               DepthWriteEnable;
        RHICompareOp                            DepthCompareOp;
        RHIBool32                               DepthBoundsTestEnable;
        RHIBool32                               StencilTestEnable;
        RHIStencilOpState                       Front;
        RHIStencilOpState                       Back;
        float                                   MinDepthBounds;
        float                                   MaxDepthBounds;
    };

    struct RHIPipelineDynamicStateCreateInfo
    {
        RHIStructureType                   SType;
        const void*                        PNext;
        RHIPipelineDynamicStateCreateFlags Flags;
        uint32_t                           DynamicStateCount;
        const RHIDynamicState*             PDynamicStates;
    };

    struct RHIPipelineInputAssemblyStateCreateInfo
    {
        RHIStructureType                         SType;
        const void*                              PNext;
        RHIPipelineInputAssemblyStateCreateFlags Flags;
        RHIPrimitiveTopology                     Topology;
        RHIBool32                                PrimitiveRestartEnable;
    };

    struct RHIPipelineLayoutCreateInfo
    {
        RHIStructureType               SType;
        const void*                    PNext;
        RHIPipelineLayoutCreateFlags   Flags;
        uint32_t                       SetLayoutCount;
        RHIDescriptorSetLayout* const* PSetLayouts;
        uint32_t                       PushConstantRangeCount;
        const RHIPushConstantRange*    PPushConstantRanges;
    };

    struct RHIPipelineMultisampleStateCreateInfo
    {
        RHIStructureType                       SType;
        const void*                            PNext;
        RHIPipelineMultisampleStateCreateFlags Flags;
        RHISampleCountFlagBits                 RasterizationSamples;
        RHIBool32                              SampleShadingEnable;
        float                                  MinSampleShading;
        const RHISampleMask**                  PSampleMask;
        RHIBool32                              AlphaToCoverageEnable;
        RHIBool32                              AlphaToOneEnable;
    };

    struct RHIPipelineRasterizationStateCreateInfo
    {
        RHIStructureType                         SType;
        const void*                              PNext;
        RHIPipelineRasterizationStateCreateFlags Flags;
        RHIBool32                                DepthClampEnable;
        RHIBool32                                RasterizerDiscardEnable;
        RHIPolygonMode                           PolygonMode;
        RHICullModeFlags                         CullMode;
        RHIFrontFace                             FrontFace;
        RHIBool32                                DepthBiasEnable;
        float                                    DepthBiasConstantFactor;
        float                                    DepthBiasClamp;
        float                                    DepthBiasSlopeFactor;
        float                                    LineWidth;
    };

    struct RHIPipelineShaderStageCreateInfo
    {
        RHIStructureType                  SType;
        const void*                       PNext;
        RHIPipelineShaderStageCreateFlags Flags;
        RHIShaderStageFlagBits            Stage;
        RHIShader*                        Module;
        const char*                       PName;
        const RHISpecializationInfo*      PSpecializationInfo;
    };

    struct RHIPipelineTessellationStateCreateInfo
    {
        RHIStructureType                        SType;
        const void*                             PNext;
        RHIPipelineTessellationStateCreateFlags Flags;
        uint32_t                                PatchControlPoints;
    };

    struct RHIPipelineVertexInputStateCreateInfo
    {
        RHIStructureType                          SType;
        const void*                               PNext;
        RHIPipelineVertexInputStateCreateFlags    Flags;
        uint32_t                                  VertexBindingDescriptionCount;
        const RHIVertexInputBindingDescription*   PVertexBindingDescriptions;
        uint32_t                                  VertexAttributeDescriptionCount;
        const RHIVertexInputAttributeDescription* PVertexAttributeDescriptions;
    };

    struct RHIPipelineViewportStateCreateInfo
    {
        RHIStructureType                    SType;
        const void*                         PNext;
        RHIPipelineViewportStateCreateFlags Flags;
        uint32_t                            ViewportCount;
        const RHIViewport*                  PViewports;
        uint32_t                            ScissorCount;
        const RHIRect2D*                    PScissors;
    };

    struct RHIPushConstantRange
    {
        RHIShaderStageFlags StageFlags;
        uint32_t            Offset;
        uint32_t            Size;
    };

    struct RHIQueueFamilyProperties
    {
        RHIQueueFlags QueueFlags;
        uint32_t      QueueCount;
        uint32_t      TimestampValidBits;
        RHIExtent3D   MinImageTransferGranularity;
    };

    struct RHIRenderPassCreateInfo
    {
        RHIStructureType                SType;
        const void*                     PNext;
        RHIRenderPassCreateFlags        Flags;
        uint32_t                        AttachmentCount;
        const RHIAttachmentDescription* PAttachments;
        uint32_t                        SubpassCount;
        const RHISubpassDescription*    PSubpasses;
        uint32_t                        DependencyCount;
        const RHISubpassDependency*     PDependencies;
    };

    struct RHISamplerCreateInfo
    {
        RHIStructureType      SType;
        const void*           PNext;
        RHISamplerCreateFlags Flags;
        RHIFilter             MagFilter;
        RHIFilter             MinFilter;
        RHISamplerMipmapMode  MipmapMode;
        RHISamplerAddressMode AddressModeU;
        RHISamplerAddressMode AddressModeV;
        RHISamplerAddressMode AddressModeW;
        float                 MipLodBias;
        RHIBool32             AnisotropyEnable;
        float                 MaxAnisotropy;
        RHIBool32             CompareEnable;
        RHICompareOp          CompareOp;
        float                 MinLod;
        float                 MaxLod;
        RHIBorderColor        BorderColor;
        RHIBool32             UnnormalizedCoordinates;
    };

    struct RHISemaphoreCreateInfo
    {
        RHIStructureType        SType;
        const void*             PNext;
        RHISemaphoreCreateFlags Flags;
    };

    struct RHIShaderModuleCreateInfo
    {
        RHIStructureType           SType;
        const void*                PNext;
        RHIShaderModuleCreateFlags Flags;
        size_t                     CodeSize;
        const uint32_t*            PCode;
    };

    struct RHISubmitInfo
    {
        RHIStructureType             SType;
        const void*                  PNext;
        uint32_t                     WaitSemaphoreCount;
        RHISemaphore**               PWaitSemaphores;
        const RHIPipelineStageFlags* PWaitDstStageMask;
        uint32_t                     CommandBufferCount;
        RHICommandBuffer* const*     PCommandBuffers;
        uint32_t                     SignalSemaphoreCount;
        const RHISemaphore**         PSignalSemaphores;
    };

    struct RHISubpassDependency
    {
        uint32_t              SrcSubpass;
        uint32_t              DstSubpass;
        RHIPipelineStageFlags SrcStageMask;
        RHIPipelineStageFlags DstStageMask;
        RHIAccessFlags        SrcAccessMask;
        RHIAccessFlags        DstAccessMask;
        RHIDependencyFlags    DependencyFlags;
    };

    struct RHISubpassDescription
    {
        RHISubpassDescriptionFlags    Flags;
        RHIPipelineBindPoint          PipelineBindPoint;
        uint32_t                      InputAttachmentCount;
        const RHIAttachmentReference* PInputAttachments;
        uint32_t                      ColorAttachmentCount;
        const RHIAttachmentReference* PColorAttachments;
        const RHIAttachmentReference* PResolveAttachments;
        const RHIAttachmentReference* PDepthStencilAttachment;
        uint32_t                      PreserveAttachmentCount;
        const uint32_t*               PPreserveAttachments;
    };

    struct RHIWriteDescriptorSet
    {
        RHIStructureType         SType;
        const void*              PNext;
        RHIDescriptorSet*        DstSet;
        uint32_t                 DstBinding;
        uint32_t                 DstArrayElement;
        uint32_t                 DescriptorCount;
        RHIDescriptorType        DescriptorType;
        RHIDescriptorImageInfo*  PImageInfo       = nullptr;
        RHIDescriptorBufferInfo* PBufferInfo      = nullptr;
        RHIBufferView*           PTexelBufferView = nullptr;
    };

    struct RHIAttachmentReference
    {
        uint32_t       Attachment;
        RHIImageLayout Layout;
    };

    struct RHIPipelineColorBlendAttachmentState
    {
        RHIBool32              BlendEnable;
        RHIBlendFactor         SrcColorBlendFactor;
        RHIBlendFactor         DstColorBlendFactor;
        RHIBlendOp             ColorBlendOp;
        RHIBlendFactor         SrcAlphaBlendFactor;
        RHIBlendFactor         DstAlphaBlendFactor;
        RHIBlendOp             AlphaBlendOp;
        RHIColorComponentFlags ColorWriteMask;
    };

    struct RHIRect2D
    {
        RHIOffset2D Offset;
        RHIExtent2D Extent;
    };

    struct RHISpecializationInfo
    {
        uint32_t                          MapEntryCount;
        const RHISpecializationMapEntry** PMapEntries;
        size_t                            DataSize;
        const void*                       PData;
    };

    struct RHIVertexInputAttributeDescription
    {
        uint32_t  Location;
        uint32_t  Binding;
        RHIFormat Format;
        uint32_t  Offset;
    };

    struct RHIVertexInputBindingDescription
    {
        uint32_t           Binding;
        uint32_t           Stride;
        RHIVertexInputRate InputRate;
    };

    struct RHIViewport
    {
        float X;
        float Y;
        float Width;
        float Height;
        float MinDepth;
        float MaxDepth;
    };

    struct RHIRenderPassBeginInfo
    {
        RHIStructureType     SType;
        const void*          PNext;
        RHIRenderPass*       RenderPass;
        RHIFramebuffer*      Framebuffer;
        RHIRect2D            RenderArea;
        uint32_t             ClearValueCount;
        const RHIClearValue* PClearValues;
    };

    struct RHIClearDepthStencilValue
    {
        float    Depth;
        uint32_t Stencil;
    };

    union RHIClearColorValue
    {
        float    Float32[4];
        int32_t  Int32[4];
        uint32_t Uint32[4];
    };

    union RHIClearValue
    {
        RHIClearColorValue        Color;
        RHIClearDepthStencilValue DepthStencil;
    };

    struct RHIClearRect
    {
        RHIRect2D Rect;
        uint32_t  BaseArrayLayer;
        uint32_t  LayerCount;
    };

    struct RHIClearAttachment
    {
        RHIImageAspectFlags AspectMask;
        uint32_t            ColorAttachment;
        RHIClearValue       ClearValue;
    };

    struct RHISwapChainDesc
    {
        RHIExtent2D                Extent;
        RHIFormat                  ImageFormat;
        RHIViewport*               Viewport;
        RHIRect2D*                 Scissor;
        std::vector<RHIImageView*> ImageViews;
    };

    struct RHIDepthImageDesc
    {
        RHIImage*     DepthImage      = RHI_NULL_HANDLE;
        RHIImageView* DepthImageView = RHI_NULL_HANDLE;
        RHIFormat     DepthImageFormat;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;
        std::optional<uint32_t> MComputeFamily;

        bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value() && MComputeFamily.has_value();
        }
    };

    //    struct SwapChainSupportDetails
    //    {
    //        VkSurfaceCapabilitiesKHR        capabilities;
    //        std::vector<VkSurfaceFormatKHR> formats;
    //        std::vector<VkPresentModeKHR>   presentModes;
    //    };
} // namespace Galaxy