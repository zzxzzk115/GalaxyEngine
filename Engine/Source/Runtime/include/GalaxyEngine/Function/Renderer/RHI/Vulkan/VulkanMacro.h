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