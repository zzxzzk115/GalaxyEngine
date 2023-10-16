//
// VulkanMacro.h
//
// Created or modified by Kexuan Zhang on 2023/10/16 12:08.
//

#pragma once

#include "GalaxyEngine/Core/Macro.h"

#define VK_CHECK(result, errorMessage) \
    do \
    { \
        VkResult err = result; \
        if (err) \
        { \
            GAL_CORE_ERROR(errorMessage); \
            GAL_CORE_ERROR("[VK_CHECK] Detected Vulkan error: {0}", (int)err); \
            GAL_CORE_ASSERT(false); \
            abort(); \
        } \
    } while (0)