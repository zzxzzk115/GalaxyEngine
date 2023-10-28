//
// RenderResource.cpp
//
// Created or modified by Kexuan Zhang on 28/10/2023.
//

#include "GalaxyEngine/Function/Renderer/Resource/RenderResource.h"
#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

namespace Galaxy
{
    void RenderResource::Clear()
    {

    }

    void RenderResource::UploadGlobalRenderResource(const Ref<RHI>& rhi)
    {
        CreateAndMapStorageBuffer(rhi);
    }

    void RenderResource::CreateAndMapStorageBuffer(const Ref<RHI>& rhi)
    {
        StorageBuffer& storageBuffer = GlobalRenderRes.Buffer;

        // In Vulkan, the storage buffer should be pre-allocated.
        // The size is 128MB in NVIDIA D3D11
        // driver(https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0).
        uint32_t global_storage_buffer_size = 1024 * 1024 * 128;
        rhi->CreateBuffer(global_storage_buffer_size,
                          RHI_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          RHI_MEMORY_PROPERTY_HOST_VISIBLE_BIT | RHI_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          storageBuffer.GlobalUploadRingBuffer,
                          storageBuffer.GlobalUploadRingBufferMemory);
    }
} // namespace Galaxy