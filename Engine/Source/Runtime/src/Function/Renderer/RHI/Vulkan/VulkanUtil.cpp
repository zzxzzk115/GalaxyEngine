#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanUtil.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanRHI.h"
#include "GalaxyEngine/Core/Macro.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Galaxy
{
    std::unordered_map<uint32_t, VkSampler> VulkanUtil::s_mipmap_sampler_map;
    VkSampler                               VulkanUtil::s_nearest_sampler = VK_NULL_HANDLE;
    VkSampler                               VulkanUtil::s_linear_sampler  = VK_NULL_HANDLE;

    uint32_t VulkanUtil::FindMemoryType(VkPhysicalDevice      physicalDevice,
                                        uint32_t              typeFilter,
                                        VkMemoryPropertyFlags propertiesFlag)
    {
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
        for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) &&
                (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & propertiesFlag) == propertiesFlag)
            {
                return i;
            }
        }
        GAL_CORE_ERROR("[VulkanUtil] FindMemoryType error");
        return 0;
    }

    VkShaderModule VulkanUtil::CreateShaderModule(VkDevice device, const std::vector<unsigned char>& shaderCode)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = shaderCode.size();
        shaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }
        return shaderModule;
    }

    void VulkanUtil::CreateBufferAndInitialize(VkDevice              device,
                                               VkPhysicalDevice      physicalDevice,
                                               VkBufferUsageFlags    usageFlags,
                                               VkMemoryPropertyFlags memoryPropertyFlags,
                                               VkBuffer*             buffer,
                                               VkDeviceMemory*       memory,
                                               VkDeviceSize          size,
                                               void*                 data,
                                               int                   datasize)
    {
        // Create the buffer handle
        VkBufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage       = usageFlags;
        bufferCreateInfo.size        = size;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (VK_SUCCESS != vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer))
        {
            GAL_CORE_ERROR("[VulkanUtil] create buffer buffer failed!");
            return;
        }

        // Create the memory backing up the buffer handle
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
        VkMemoryRequirements memReqs;
        VkMemoryAllocateInfo memAlloc {};
        memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkGetBufferMemoryRequirements(device, *buffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;

        // Find a memory type index that fits the properties of the buffer
        bool memTypeFound = false;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
        {
            if ((memReqs.memoryTypeBits & 1) == 1)
            {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
                {
                    memAlloc.memoryTypeIndex = i;
                    memTypeFound             = true;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (!memTypeFound)
        {
            GAL_CORE_ERROR("[VulkanUtil] memTypeFound is nullptr");
            return;
        }
        if (VK_SUCCESS != vkAllocateMemory(device, &memAlloc, nullptr, memory))
        {
            GAL_CORE_ERROR("[VulkanUtil] alloc memory failed!");
            return;
        }

        if (data != nullptr && datasize != 0)
        {
            void* mapped;
            if (VK_SUCCESS != vkMapMemory(device, *memory, 0, size, 0, &mapped))
            {
                GAL_CORE_ERROR("[VulkanUtil] map memory failed!");
                return;
            }
            memcpy(mapped, data, datasize);
            vkUnmapMemory(device, *memory);
        }

        if (VK_SUCCESS != vkBindBufferMemory(device, *buffer, *memory, 0))
        {
            GAL_CORE_ERROR("[VulkanUtil] bind memory failed!");
            return;
        }
    }

    void VulkanUtil::CreateBuffer(VkPhysicalDevice      physicalDevice,
                                  VkDevice              device,
                                  VkDeviceSize          size,
                                  VkBufferUsageFlags    usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer&             buffer,
                                  VkDeviceMemory&       bufferMemory)
    {
        VkBufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size        = size;
        bufferCreateInfo.usage       = usage;                     // use as a vertex/staging/index buffer
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // not sharing among queue families

        if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanUtil] vkCreateBuffer failed!");
            return;
        }

        VkMemoryRequirements bufferMemoryRequirements; // for allocate_info.allocationSize and
                                                         // allocate_info.memoryTypeIndex
        vkGetBufferMemoryRequirements(device, buffer, &bufferMemoryRequirements);

        VkMemoryAllocateInfo bufferMemoryAllocateInfo {};
        bufferMemoryAllocateInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        bufferMemoryAllocateInfo.allocationSize = bufferMemoryRequirements.size;
        bufferMemoryAllocateInfo.memoryTypeIndex =
            VulkanUtil::FindMemoryType(physicalDevice, bufferMemoryRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &bufferMemoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanUtil] vkAllocateMemory failed!");
            return;
        }

        // bind buffer with buffer memory
        vkBindBufferMemory(device, buffer, bufferMemory, 0); // offset = 0
    }

    void VulkanUtil::CopyBuffer(RHI*         rhi,
                                VkBuffer     srcBuffer,
                                VkBuffer     dstBuffer,
                                VkDeviceSize srcOffset,
                                VkDeviceSize dstOffset,
                                VkDeviceSize size)
    {
        if (rhi == nullptr)
        {
            GAL_CORE_ERROR("[VulkanUtil] rhi is nullptr");
            return;
        }

        RHICommandBuffer* rhiCommandBuffer = static_cast<VulkanRHI*>(rhi)->BeginSingleTimeCommands();
        VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)rhiCommandBuffer)->GetResource();

        VkBufferCopy copyRegion = {srcOffset, dstOffset, size};
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        static_cast<VulkanRHI*>(rhi)->EndSingleTimeCommands(rhiCommandBuffer);
    }

    void VulkanUtil::CreateImage(VkPhysicalDevice      physicalDevice,
                                 VkDevice              device,
                                 uint32_t              imageWidth,
                                 uint32_t              imageHeight,
                                 VkFormat              format,
                                 VkImageTiling         imageTiling,
                                 VkImageUsageFlags     imageUsageFlags,
                                 VkMemoryPropertyFlags memoryPropertyFlags,
                                 VkImage&              image,
                                 VkDeviceMemory&       memory,
                                 VkImageCreateFlags    imageCreateFlags,
                                 uint32_t              arrayLayers,
                                 uint32_t              miplevels)
    {
        VkImageCreateInfo imageCreateInfo {};
        imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags         = imageCreateFlags;
        imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width  = imageWidth;
        imageCreateInfo.extent.height = imageHeight;
        imageCreateInfo.extent.depth  = 1;
        imageCreateInfo.mipLevels     = miplevels;
        imageCreateInfo.arrayLayers   = arrayLayers;
        imageCreateInfo.format        = format;
        imageCreateInfo.tiling        = imageTiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage         = imageUsageFlags;
        imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanUtil] failed to create image!");
            return;
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, memoryPropertyFlags);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        {
            GAL_CORE_ERROR("[VulkanUtil] failed to allocate image memory!");
            return;
        }

        vkBindImageMemory(device, image, memory, 0);
    }

    VkImageView VulkanUtil::CreateImageView(VkDevice           device,
                                            VkImage&           image,
                                            VkFormat           format,
                                            VkImageAspectFlags imageAspectFlags,
                                            VkImageViewType    viewType,
                                            uint32_t           layoutCount,
                                            uint32_t           miplevels)
    {
        VkImageViewCreateInfo imageViewCreateInfo {};
        imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image                           = image;
        imageViewCreateInfo.viewType                        = viewType;
        imageViewCreateInfo.format                          = format;
        imageViewCreateInfo.subresourceRange.aspectMask     = imageAspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        imageViewCreateInfo.subresourceRange.levelCount     = miplevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount     = layoutCount;

        VkImageView imageView;
        if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            return imageView;
            // todo
        }

        return imageView;
    }

    void VulkanUtil::CreateGlobalImage(RHI*               rhi,
                                       VkImage&           image,
                                       VkImageView&       imageView,
                                       VmaAllocation&     imageAllocation,
                                       uint32_t           textureImageWidth,
                                       uint32_t           textureImageHeight,
                                       void*              textureImagePixels,
                                       RHIFormat textureImageFormat,
                                       uint32_t           miplevels)
    {
        if (!textureImagePixels)
        {
            return;
        }

        VkDeviceSize textureByteSize;
        VkFormat     vulkanImageFormat;
        switch (textureImageFormat)
        {
            case RHIFormat::RHI_FORMAT_R8G8B8_UNORM:
                textureByteSize   = textureImageWidth * textureImageHeight * 3;
                vulkanImageFormat = VK_FORMAT_R8G8B8_UNORM;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8_SRGB:
                textureByteSize   = textureImageWidth * textureImageHeight * 3;
                vulkanImageFormat = VK_FORMAT_R8G8B8_SRGB;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8A8_UNORM:
                textureByteSize   = textureImageWidth * textureImageHeight * 4;
                vulkanImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8A8_SRGB:
                textureByteSize   = textureImageWidth * textureImageHeight * 4;
                vulkanImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
                break;
            case RHIFormat::RHI_FORMAT_R32_SFLOAT:
                textureByteSize = textureImageWidth * textureImageHeight * 4;
                vulkanImageFormat = VK_FORMAT_R32_SFLOAT;
                break;
            case RHIFormat::RHI_FORMAT_R32G32_SFLOAT:
                textureByteSize   = textureImageWidth * textureImageHeight * 4 * 2;
                vulkanImageFormat = VK_FORMAT_R32G32_SFLOAT;
                break;
            case RHIFormat::RHI_FORMAT_R32G32B32_SFLOAT:
                textureByteSize   = textureImageWidth * textureImageHeight * 4 * 3;
                vulkanImageFormat = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case RHIFormat::RHI_FORMAT_R32G32B32A32_SFLOAT:
                textureByteSize   = textureImageWidth * textureImageHeight * 4 * 4;
                vulkanImageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                GAL_CORE_ERROR("[VulkanUtil] invalid texture_byte_size");
                break;
        }

        // use staging buffer
        VkBuffer       inefficientStagingBuffer;
        VkDeviceMemory inefficientStagingBufferMemory;
        VulkanUtil::CreateBuffer(static_cast<VulkanRHI*>(rhi)->PhysicalDevice,
                                 static_cast<VulkanRHI*>(rhi)->Device,
                                 textureByteSize,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 inefficientStagingBuffer,
                                 inefficientStagingBufferMemory);

        void* data;
        vkMapMemory(
            static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory, 0, textureByteSize, 0, &data);
        memcpy(data, textureImagePixels, static_cast<size_t>(textureByteSize));
        vkUnmapMemory(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory);

        // generate mipmapped image
        uint32_t mipLevels =
            (miplevels != 0) ? miplevels : floor(log2(std::max(textureImageWidth, textureImageHeight))) + 1;

        // use the vmaAllocator to allocate asset texture image
        VkImageCreateInfo imageCreateInfo {};
        imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags         = 0;
        imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width  = textureImageWidth;
        imageCreateInfo.extent.height = textureImageHeight;
        imageCreateInfo.extent.depth  = 1;
        imageCreateInfo.mipLevels     = mipLevels;
        imageCreateInfo.arrayLayers   = 1;
        imageCreateInfo.format        = vulkanImageFormat;
        imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage =
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(static_cast<VulkanRHI*>(rhi)->AssetsAllocator,
                       &imageCreateInfo,
                       &allocInfo,
                       &image,
                       &imageAllocation,
                       nullptr);

        // layout transitions -- image layout is set from none to destination
        TransitionImageLayout(rhi,
                              image,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              1,
                              1,
                              VK_IMAGE_ASPECT_COLOR_BIT);
        // copy from staging buffer as destination
        CopyBufferToImage(rhi, inefficientStagingBuffer, image, textureImageWidth, textureImageHeight, 1);
        // layout transitions -- image layout is set from destination to shader_read
        TransitionImageLayout(rhi,
                              image,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              1,
                              1,
                              VK_IMAGE_ASPECT_COLOR_BIT);

        vkDestroyBuffer(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBuffer, nullptr);
        vkFreeMemory(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory, nullptr);

        // generate mipmapped image
        GenMipmappedImage(rhi, image, textureImageWidth, textureImageHeight, mipLevels);

        imageView = CreateImageView(static_cast<VulkanRHI*>(rhi)->Device,
                                     image,
                                     vulkanImageFormat,
                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                     VK_IMAGE_VIEW_TYPE_2D,
                                     1,
                                     mipLevels);
    }

    void VulkanUtil::CreateCubeMap(RHI*                 rhi,
                                   VkImage&             image,
                                   VkImageView&         imageView,
                                   VmaAllocation&       imageAllocation,
                                   uint32_t             textureImageWidth,
                                   uint32_t             textureImageHeight,
                                   std::array<void*, 6> textureImagePixels,
                                   RHIFormat   textureImageFormat,
                                   uint32_t             miplevels)
    {
        VkDeviceSize textureLayerByteSize;
        VkDeviceSize cubeByteSize;
        VkFormat     vulkanImageFormat;
        switch (textureImageFormat)
        {
            case RHIFormat::RHI_FORMAT_R8G8B8_UNORM:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 3;
                vulkanImageFormat     = VK_FORMAT_R8G8B8_UNORM;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8_SRGB:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 3;
                vulkanImageFormat     = VK_FORMAT_R8G8B8_SRGB;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8A8_UNORM:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 4;
                vulkanImageFormat     = VK_FORMAT_R8G8B8A8_UNORM;
                break;
            case RHIFormat::RHI_FORMAT_R8G8B8A8_SRGB:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 4;
                vulkanImageFormat     = VK_FORMAT_R8G8B8A8_SRGB;
                break;
            case RHIFormat::RHI_FORMAT_R32G32_SFLOAT:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 4 * 2;
                vulkanImageFormat     = VK_FORMAT_R32G32_SFLOAT;
                break;
            case RHIFormat::RHI_FORMAT_R32G32B32_SFLOAT:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 4 * 3;
                vulkanImageFormat     = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case RHIFormat::RHI_FORMAT_R32G32B32A32_SFLOAT:
                textureLayerByteSize = textureImageWidth * textureImageHeight * 4 * 4;
                vulkanImageFormat     = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                textureLayerByteSize = VkDeviceSize(-1);
                GAL_CORE_ERROR("[VulkanUtil] invalid texture_layer_byte_size");
                return;
                break;
        }

        cubeByteSize = textureLayerByteSize * 6;

        // create cubemap texture image
        // use the vmaAllocator to allocate asset texture image
        VkImageCreateInfo imageCreateInfo {};
        imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags         = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width  = static_cast<uint32_t>(textureImageWidth);
        imageCreateInfo.extent.height = static_cast<uint32_t>(textureImageHeight);
        imageCreateInfo.extent.depth  = 1;
        imageCreateInfo.mipLevels     = miplevels;
        imageCreateInfo.arrayLayers   = 6;
        imageCreateInfo.format        = vulkanImageFormat;
        imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage =
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageCreateInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(static_cast<VulkanRHI*>(rhi)->AssetsAllocator,
                       &imageCreateInfo,
                       &allocInfo,
                       &image,
                       &imageAllocation,
                       nullptr);

        VkBuffer       inefficientStagingBuffer;
        VkDeviceMemory inefficientStagingBufferMemory;
        CreateBuffer(static_cast<VulkanRHI*>(rhi)->PhysicalDevice,
                     static_cast<VulkanRHI*>(rhi)->Device,
                     cubeByteSize,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     inefficientStagingBuffer,
                     inefficientStagingBufferMemory);

        void* data = nullptr;
        vkMapMemory(
            static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory, 0, cubeByteSize, 0, &data);
        for (int i = 0; i < 6; i++)
        {
            memcpy((void*)(static_cast<char*>(data) + textureLayerByteSize * i),
                   textureImagePixels[i],
                   static_cast<size_t>(textureLayerByteSize));
        }
        vkUnmapMemory(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory);

        // layout transitions -- image layout is set from none to destination
        TransitionImageLayout(rhi,
                              image,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              6,
                              miplevels,
                              VK_IMAGE_ASPECT_COLOR_BIT);
        // copy from staging buffer as destination
        CopyBufferToImage(rhi,
                          inefficientStagingBuffer,
                          image,
                          static_cast<uint32_t>(textureImageWidth),
                          static_cast<uint32_t>(textureImageHeight),
                          6);

        vkDestroyBuffer(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBuffer, nullptr);
        vkFreeMemory(static_cast<VulkanRHI*>(rhi)->Device, inefficientStagingBufferMemory, nullptr);

        GenerateTextureMipMaps(
            rhi, image, vulkanImageFormat, textureImageWidth, textureImageHeight, 6, miplevels);

        imageView = CreateImageView(static_cast<VulkanRHI*>(rhi)->Device,
                                     image,
                                     vulkanImageFormat,
                                     VK_IMAGE_ASPECT_COLOR_BIT,
                                     VK_IMAGE_VIEW_TYPE_CUBE,
                                     6,
                                     miplevels);
    }

    void VulkanUtil::GenerateTextureMipMaps(RHI*     rhi,
                                            VkImage  image,
                                            VkFormat imageFormat,
                                            uint32_t textureWidth,
                                            uint32_t textureHeight,
                                            uint32_t layers,
                                            uint32_t miplevels)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(
            static_cast<VulkanRHI*>(rhi)->PhysicalDevice, imageFormat, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            GAL_CORE_ERROR("[VulkanUtil] GenerateTextureMipMaps() : linear bliting not supported!");
            return;
        }

        RHICommandBuffer* rhiCommandBuffer = static_cast<VulkanRHI*>(rhi)->BeginSingleTimeCommands();
        VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)rhiCommandBuffer)->GetResource();

        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image                           = image;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = layers;
        barrier.subresourceRange.levelCount     = 1; // 1 level a time

        int32_t mipwidth  = textureWidth;
        int32_t mipheight = textureHeight;

        for (uint32_t i = 1; i < miplevels; i++) // use miplevel i-1 to generate miplevel i and set i-1 to
                                                 // shader_read
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            VkImageBlit blit {};
            blit.srcOffsets[0]                 = {0, 0, 0};
            blit.srcOffsets[1]                 = {mipwidth, mipheight, 1};
            blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel       = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount     = layers; // miplevel i-1 to i for all layers

            blit.dstOffsets[0]             = {0, 0, 0};
            blit.dstOffsets[1]             = {mipwidth > 1 ? mipwidth / 2 : 1, mipheight > 1 ? mipheight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel   = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount     = layers;

            vkCmdBlitImage(commandBuffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier); // for completed miplevel, change to shader_read

            if (mipwidth > 1)
                mipwidth /= 2;
            if (mipheight > 1)
                mipheight /= 2;
        }

        // the last miplevel(miplevels - 1) change to shader_read
        barrier.subresourceRange.baseMipLevel = miplevels - 1;
        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        static_cast<VulkanRHI*>(rhi)->EndSingleTimeCommands(rhiCommandBuffer);
    }

    void VulkanUtil::TransitionImageLayout(RHI*               rhi,
                                           VkImage            image,
                                           VkImageLayout      oldLayout,
                                           VkImageLayout      newLayout,
                                           uint32_t           layerCount,
                                           uint32_t           miplevels,
                                           VkImageAspectFlags aspectMaskBits)
    {
        if (rhi == nullptr)
        {
            GAL_CORE_ERROR("[VulkanUtil] rhi is nullptr");
            return;
        }

        RHICommandBuffer* rhiCommandBuffer = static_cast<VulkanRHI*>(rhi)->BeginSingleTimeCommands();
        VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)rhiCommandBuffer)->GetResource();

        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = oldLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = aspectMaskBits;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = miplevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = layerCount;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        // for getGuidAndDepthOfMouseClickOnRenderSceneForUI() get depthimage
        else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        // for generating mipmapped image
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else
        {
            GAL_CORE_ERROR("[VulkanUtil] unsupported layout transition!");
            return;
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        static_cast<VulkanRHI*>(rhi)->EndSingleTimeCommands(rhiCommandBuffer);
    }

    void VulkanUtil::CopyBufferToImage(RHI*     rhi,
                                       VkBuffer buffer,
                                       VkImage  image,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t layerCount)
    {
        if (rhi == nullptr)
        {
            GAL_CORE_ERROR("[VulkanUtil] rhi is nullptr");
            return;
        }

        RHICommandBuffer* rhiCommandBuffer = static_cast<VulkanRHI*>(rhi)->BeginSingleTimeCommands();
        VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)rhiCommandBuffer)->GetResource();

        VkBufferImageCopy region {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = layerCount;
        region.imageOffset                     = {0, 0, 0};
        region.imageExtent                     = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        static_cast<VulkanRHI*>(rhi)->EndSingleTimeCommands(rhiCommandBuffer);
    }

    void VulkanUtil::GenMipmappedImage(RHI* rhi, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevels)
    {
        if (rhi == nullptr)
        {
            GAL_CORE_ERROR("[VulkanUtil] rhi is nullptr");
            return;
        }

        RHICommandBuffer* rhiCommandBuffer = static_cast<VulkanRHI*>(rhi)->BeginSingleTimeCommands();
        VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)rhiCommandBuffer)->GetResource();

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            VkImageBlit imageBlit {};
            imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.srcSubresource.layerCount = 1;
            imageBlit.srcSubresource.mipLevel   = i - 1;
            imageBlit.srcOffsets[1].x           = std::max((int32_t)(width >> (i - 1)), 1);
            imageBlit.srcOffsets[1].y           = std::max((int32_t)(height >> (i - 1)), 1);
            imageBlit.srcOffsets[1].z           = 1;

            imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageBlit.dstSubresource.layerCount = 1;
            imageBlit.dstSubresource.mipLevel   = i;
            imageBlit.dstOffsets[1].x           = std::max((int32_t)(width >> i), 1);
            imageBlit.dstOffsets[1].y           = std::max((int32_t)(height >> i), 1);
            imageBlit.dstOffsets[1].z           = 1;

            VkImageSubresourceRange mipSubRange {};
            mipSubRange.aspectMask   = VK_IMAGE_ASPECT_COLOR_BIT;
            mipSubRange.baseMipLevel = i;
            mipSubRange.levelCount   = 1;
            mipSubRange.layerCount   = 1;

            VkImageMemoryBarrier barrier {};
            barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcAccessMask       = 0;
            barrier.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image               = image;
            barrier.subresourceRange    = mipSubRange;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            vkCmdBlitImage(commandBuffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &imageBlit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);
        }

        VkImageSubresourceRange mipSubRange {};
        mipSubRange.aspectMask   = VK_IMAGE_ASPECT_COLOR_BIT;
        mipSubRange.baseMipLevel = 0;
        mipSubRange.levelCount   = mipLevels;
        mipSubRange.layerCount   = 1;

        VkImageMemoryBarrier barrier {};
        barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask       = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = image;
        barrier.subresourceRange    = mipSubRange;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        static_cast<VulkanRHI*>(rhi)->EndSingleTimeCommands(rhiCommandBuffer);
    }

    VkSampler VulkanUtil::GetOrCreateMipmapSampler(VkPhysicalDevice physicalDevice,
                                                   VkDevice         device,
                                                   uint32_t         width,
                                                   uint32_t         height)
    {
        if (width <= 0 || height <= 0)
        {
            GAL_CORE_ERROR("[VulkanUtil] width <= 0 || height <= 0");
        }

        VkSampler sampler;
        uint32_t  mipLevels   = floor(log2(std::max(width, height))) + 1;
        auto      findSampler = s_mipmap_sampler_map.find(mipLevels);
        if (findSampler != s_mipmap_sampler_map.end())
        {
            return findSampler->second;
        }
        else
        {
            VkPhysicalDeviceProperties physicalDeviceProperties {};
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            VkSamplerCreateInfo samplerInfo {};
            samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter    = VK_FILTER_LINEAR;
            samplerInfo.minFilter    = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy    = physicalDeviceProperties.limits.maxSamplerAnisotropy;

            samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable           = VK_FALSE;
            samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;

            samplerInfo.maxLod = mipLevels - 1;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
            {
                GAL_CORE_ERROR("[VulkanUtil] vkCreateSampler failed!");
            }
        }

        s_mipmap_sampler_map.insert(std::make_pair(mipLevels, sampler));

        return sampler;
    }

    void VulkanUtil::DestroyMipmappedSampler(VkDevice device)
    {
        for (auto sampler : s_mipmap_sampler_map)
        {
            vkDestroySampler(device, sampler.second, nullptr);
        }
        s_mipmap_sampler_map.clear();
    }

    VkSampler VulkanUtil::GetOrCreateNearestSampler(VkPhysicalDevice physicalDevice, VkDevice device)
    {
        if (s_nearest_sampler == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties physicalDeviceProperties {};
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            VkSamplerCreateInfo samplerInfo {};

            samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter               = VK_FILTER_NEAREST;
            samplerInfo.minFilter               = VK_FILTER_NEAREST;
            samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.mipLodBias              = 0.0f;
            samplerInfo.anisotropyEnable        = VK_FALSE;
            samplerInfo.maxAnisotropy           = physicalDeviceProperties.limits.maxSamplerAnisotropy; // close :1.0f
            samplerInfo.compareEnable           = VK_FALSE;
            samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
            samplerInfo.minLod                  = 0.0f;
            samplerInfo.maxLod                  = 8.0f; // todo: s_irradiance_texture_miplevels
            samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &s_nearest_sampler) != VK_SUCCESS)
            {
                GAL_CORE_ERROR("[VulkanUtil] vk create sampler");
            }
        }

        return s_nearest_sampler;
    }

    VkSampler VulkanUtil::GetOrCreateLinearSampler(VkPhysicalDevice physicalDevice, VkDevice device)
    {
        if (s_linear_sampler == VK_NULL_HANDLE)
        {
            VkPhysicalDeviceProperties physicalDeviceProperties {};
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

            VkSamplerCreateInfo samplerInfo {};

            samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter               = VK_FILTER_LINEAR;
            samplerInfo.minFilter               = VK_FILTER_LINEAR;
            samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.mipLodBias              = 0.0f;
            samplerInfo.anisotropyEnable        = VK_FALSE;
            samplerInfo.maxAnisotropy           = physicalDeviceProperties.limits.maxSamplerAnisotropy; // close :1.0f
            samplerInfo.compareEnable           = VK_FALSE;
            samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
            samplerInfo.minLod                  = 0.0f;
            samplerInfo.maxLod                  = 8.0f; // todo: s_irradiance_texture_miplevels
            samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &s_linear_sampler) != VK_SUCCESS)
            {
                GAL_CORE_ERROR("[VulkanUtil] vk create sampler");
            }
        }

        return s_linear_sampler;
    }

    void VulkanUtil::DestroyNearestSampler(VkDevice device)
    {
        vkDestroySampler(device, s_nearest_sampler, nullptr);
        s_nearest_sampler = VK_NULL_HANDLE;
    }

    void VulkanUtil::DestroyLinearSampler(VkDevice device)
    {
        vkDestroySampler(device, s_linear_sampler, nullptr);
        s_linear_sampler = VK_NULL_HANDLE;
    }
} // namespace Galaxy