//
// RenderResource.h
//
// Created or modified by Kexuan Zhang on 27/10/2023.
//

#pragma once

#include "GalaxyEngine/Function/Renderer/RHI/RHIStruct.h"
#include "GalaxyEngine/Function/Renderer/Resource/RenderResourceBase.h"

namespace Galaxy
{
    struct StorageBuffer
    {
        RHIBuffer* GlobalUploadRingBuffer;
        RHIDeviceMemory* GlobalUploadRingBufferMemory;
    };

    struct MeshPerFrameStorageBufferObject
    {

    };

    struct GlobalRenderResource
    {
        StorageBuffer Buffer;
    };

    class RenderResource : public RenderResourceBase
    {
    public:
        virtual void Clear() override final;
        virtual void UploadGlobalRenderResource(const Ref<RHI>& rhi) override final;

    private:
        void CreateAndMapStorageBuffer(const Ref<RHI>& rhi);

    public:
        GlobalRenderResource GlobalRenderRes;
    };
} // namespace Galaxy