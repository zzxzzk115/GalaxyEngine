//
// RenderMesh.h
//
// Created or modified by Kexuan Zhang on 27/10/2023.
//

#pragma once

#include "GalaxyEngine/Core/Math/Vector2.h"
#include "GalaxyEngine/Core/Math/Vector3.h"
#include "GalaxyEngine/Core/Math/Vector4.h"

#include <array>
#include "GalaxyEngine/Function/Renderer/RHI/RHI.h"

using namespace Galaxy::Math;

namespace Galaxy
{
    struct MeshVertex
    {
        struct MeshVertexInput
        {
            Vector3 position;
            Vector3 normal;
            Vector2 texCoord;
        };

        static std::array<RHIVertexInputBindingDescription, 1> GetBindingDescriptions()
        {
            std::array<RHIVertexInputBindingDescription, 1> bindingDescriptions {};

            bindingDescriptions[0].binding   = 0;
            bindingDescriptions[0].stride    = sizeof(MeshVertexInput);
            bindingDescriptions[0].inputRate = RHI_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescriptions;
        }

        static std::array<RHIVertexInputAttributeDescription, 3> GetAttributeDescriptions()
        {
            std::array<RHIVertexInputAttributeDescription, 3> attributeDescriptions {};

            // position
            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = RHI_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(MeshVertexInput, position);

            // normal
            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = RHI_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(MeshVertexInput, normal);

            // texCoord
            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = RHI_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(MeshVertexInput, texCoord);

            return attributeDescriptions;
        }
    };
} // namespace Galaxy