//
// VulkanUtil.h
//
// Created or modified by Kexuan Zhang on 23/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <array>
#include <unordered_map>
#include <vector>

namespace Galaxy
{
    class VulkanUtil
    {
    public:
        static uint32_t
        FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags propertiesFlag);
        static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<unsigned char>& shaderCode);
        static void           CreateBuffer(VkPhysicalDevice      physicalDevice,
                                           VkDevice              device,
                                           VkDeviceSize          size,
                                           VkBufferUsageFlags    usage,
                                           VkMemoryPropertyFlags properties,
                                           VkBuffer&             buffer,
                                           VkDeviceMemory&       bufferMemory);
        static void           CreateBufferAndInitialize(VkDevice              device,
                                                        VkPhysicalDevice      physicalDevice,
                                                        VkBufferUsageFlags    usageFlags,
                                                        VkMemoryPropertyFlags memoryPropertyFlags,
                                                        VkBuffer*             buffer,
                                                        VkDeviceMemory*       memory,
                                                        VkDeviceSize          size,
                                                        void*                 data     = nullptr,
                                                        int                   datasize = 0);
        static void           CopyBuffer(RHI*         rhi,
                                         VkBuffer     srcBuffer,
                                         VkBuffer     dstBuffer,
                                         VkDeviceSize srcOffset,
                                         VkDeviceSize dstOffset,
                                         VkDeviceSize size);
        static void           CreateImage(VkPhysicalDevice      physicalDevice,
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
                                          uint32_t              miplevels);
        static VkImageView    CreateImageView(VkDevice           device,
                                              VkImage&           image,
                                              VkFormat           format,
                                              VkImageAspectFlags imageAspectFlags,
                                              VkImageViewType    viewType,
                                              uint32_t           layoutCount,
                                              uint32_t           miplevels);
        static void           CreateGlobalImage(RHI*               rhi,
                                                VkImage&           image,
                                                VkImageView&       imageView,
                                                VmaAllocation&     imageAllocation,
                                                uint32_t           textureImageWidth,
                                                uint32_t           textureImageHeight,
                                                void*              textureImagePixels,
                                                RHIFormat textureImageFormat,
                                                uint32_t           miplevels = 0);
        static void           CreateCubeMap(RHI*                 rhi,
                                            VkImage&             image,
                                            VkImageView&         imageView,
                                            VmaAllocation&       imageAllocation,
                                            uint32_t             textureImageWidth,
                                            uint32_t             textureImageHeight,
                                            std::array<void*, 6> textureImagePixels,
                                            RHIFormat   textureImageFormat,
                                            uint32_t             miplevels);
        static void           GenerateTextureMipMaps(RHI*     rhi,
                                                     VkImage  image,
                                                     VkFormat imageFormat,
                                                     uint32_t textureWidth,
                                                     uint32_t textureHeight,
                                                     uint32_t layers,
                                                     uint32_t miplevels);
        static void           TransitionImageLayout(RHI*               rhi,
                                                    VkImage            image,
                                                    VkImageLayout      oldLayout,
                                                    VkImageLayout      newLayout,
                                                    uint32_t           layerCount,
                                                    uint32_t           miplevels,
                                                    VkImageAspectFlags aspectMaskBits);
        static void           CopyBufferToImage(RHI*     rhi,
                                                VkBuffer buffer,
                                                VkImage  image,
                                                uint32_t width,
                                                uint32_t height,
                                                uint32_t layerCount);
        static void GenMipmappedImage(RHI* rhi, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevels);

        static VkSampler
        GetOrCreateMipmapSampler(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height);
        static void      DestroyMipmappedSampler(VkDevice device);
        static VkSampler GetOrCreateNearestSampler(VkPhysicalDevice physicalDevice, VkDevice device);
        static VkSampler GetOrCreateLinearSampler(VkPhysicalDevice physicalDevice, VkDevice device);
        static void      DestroyNearestSampler(VkDevice device);
        static void      DestroyLinearSampler(VkDevice device);

    private:
        static std::unordered_map<uint32_t, VkSampler> s_mipmap_sampler_map;
        static VkSampler                               s_nearest_sampler;
        static VkSampler                               s_linear_sampler;
    };
} // namespace Galaxy