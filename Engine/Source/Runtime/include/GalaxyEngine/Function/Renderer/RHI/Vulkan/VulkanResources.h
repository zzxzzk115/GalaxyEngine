//
// VulkanResources.h
//
// Created or modified by Kexuan Zhang on 23/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

#include <vulkan/vulkan.h>
#include <optional>

namespace Galaxy
{
    class VulkanBuffer : public RHIBuffer
    {
    public:
        void     SetResource(VkBuffer res) { m_Resource = res; }
        VkBuffer GetResource() const { return m_Resource; }

    private:
        VkBuffer m_Resource;
    };
    class VulkanBufferView : public RHIBufferView
    {
    public:
        void         SetResource(VkBufferView res) { m_Resource = res; }
        VkBufferView GetResource() const { return m_Resource; }

    private:
        VkBufferView m_Resource;
    };
    class VulkanCommandBuffer : public RHICommandBuffer
    {
    public:
        void                  SetResource(VkCommandBuffer res) { m_Resource = res; }
        VkCommandBuffer GetResource() const { return m_Resource; }

    private:
        VkCommandBuffer m_Resource;
    };
    class VulkanCommandPool : public RHICommandPool
    {
    public:
        void          SetResource(VkCommandPool res) { m_Resource = res; }
        VkCommandPool GetResource() const { return m_Resource; }

    private:
        VkCommandPool m_Resource;
    };
    class VulkanDescriptorPool : public RHIDescriptorPool
    {
    public:
        void             SetResource(VkDescriptorPool res) { m_Resource = res; }
        VkDescriptorPool GetResource() const { return m_Resource; }

    private:
        VkDescriptorPool m_Resource;
    };
    class VulkanDescriptorSet : public RHIDescriptorSet
    {
    public:
        void            SetResource(VkDescriptorSet res) { m_Resource = res; }
        VkDescriptorSet GetResource() const { return m_Resource; }

    private:
        VkDescriptorSet m_Resource;
    };
    class VulkanDescriptorSetLayout : public RHIDescriptorSetLayout
    {
    public:
        void                  SetResource(VkDescriptorSetLayout res) { m_Resource = res; }
        VkDescriptorSetLayout GetResource() const { return m_Resource; }

    private:
        VkDescriptorSetLayout m_Resource;
    };
    class VulkanDevice : public RHIDevice
    {
    public:
        void     SetResource(VkDevice res) { m_Resource = res; }
        VkDevice GetResource() const { return m_Resource; }

    private:
        VkDevice m_Resource;
    };
    class VulkanDeviceMemory : public RHIDeviceMemory
    {
    public:
        void           SetResource(VkDeviceMemory res) { m_Resource = res; }
        VkDeviceMemory GetResource() const { return m_Resource; }

    private:
        VkDeviceMemory m_Resource;
    };
    class VulkanEvent : public RHIEvent
    {
    public:
        void    SetResource(VkEvent res) { m_Resource = res; }
        VkEvent GetResource() const { return m_Resource; }

    private:
        VkEvent m_Resource;
    };
    class VulkanFence : public RHIFence
    {
    public:
        void    SetResource(VkFence res) { m_Resource = res; }
        VkFence GetResource() const { return m_Resource; }

    private:
        VkFence m_Resource;
    };
    class VulkanFramebuffer : public RHIFramebuffer
    {
    public:
        void          SetResource(VkFramebuffer res) { m_Resource = res; }
        VkFramebuffer GetResource() const { return m_Resource; }

    private:
        VkFramebuffer m_Resource;
    };
    class VulkanImage : public RHIImage
    {
    public:
        void     SetResource(VkImage res) { m_Resource = res; }
        VkImage& GetResource() { return m_Resource; }

    private:
        VkImage m_Resource;
    };
    class VulkanImageView : public RHIImageView
    {
    public:
        void        SetResource(VkImageView res) { m_Resource = res; }
        VkImageView GetResource() const { return m_Resource; }

    private:
        VkImageView m_Resource;
    };
    class VulkanInstance : public RHIInstance
    {
    public:
        void       SetResource(VkInstance res) { m_Resource = res; }
        VkInstance GetResource() const { return m_Resource; }

    private:
        VkInstance m_Resource;
    };
    class VulkanQueue : public RHIQueue
    {
    public:
        void    SetResource(VkQueue res) { m_Resource = res; }
        VkQueue GetResource() const { return m_Resource; }

    private:
        VkQueue m_Resource;
    };
    class VulkanPhysicalDevice : public RHIPhysicalDevice
    {
    public:
        void             SetResource(VkPhysicalDevice res) { m_Resource = res; }
        VkPhysicalDevice GetResource() const { return m_Resource; }

    private:
        VkPhysicalDevice m_Resource;
    };
    class VulkanPipeline : public RHIPipeline
    {
    public:
        void       SetResource(VkPipeline res) { m_Resource = res; }
        VkPipeline GetResource() const { return m_Resource; }

    private:
        VkPipeline m_Resource;
    };
    class VulkanPipelineCache : public RHIPipelineCache
    {
    public:
        void            SetResource(VkPipelineCache res) { m_Resource = res; }
        VkPipelineCache GetResource() const { return m_Resource; }

    private:
        VkPipelineCache m_Resource;
    };
    class VulkanPipelineLayout : public RHIPipelineLayout
    {
    public:
        void             SetResource(VkPipelineLayout res) { m_Resource = res; }
        VkPipelineLayout GetResource() const { return m_Resource; }

    private:
        VkPipelineLayout m_Resource;
    };
    class VulkanRenderPass : public RHIRenderPass
    {
    public:
        void         SetResource(VkRenderPass res) { m_Resource = res; }
        VkRenderPass GetResource() const { return m_Resource; }

    private:
        VkRenderPass m_Resource;
    };
    class VulkanSampler : public RHISampler
    {
    public:
        void      SetResource(VkSampler res) { m_Resource = res; }
        VkSampler GetResource() const { return m_Resource; }

    private:
        VkSampler m_Resource;
    };
    class VulkanSemaphore : public RHISemaphore
    {
    public:
        void         SetResource(VkSemaphore res) { m_Resource = res; }
        VkSemaphore& GetResource() { return m_Resource; }

    private:
        VkSemaphore m_Resource;
    };
    class VulkanShader : public RHIShader
    {
    public:
        void           SetResource(VkShaderModule res) { m_Resource = res; }
        VkShaderModule GetResource() const { return m_Resource; }

    private:
        VkShaderModule m_Resource;
    };
} // namespace Galaxy