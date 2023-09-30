#pragma once

#include "Core/Base.h"
#include "Function/Renderer/RHI/Vulkan/VulkanGlobalContext.h"
#include <vulkan/vulkan.h>

namespace Galaxy
{
    class VulkanShader
    {
    public:
        VulkanShader(const std::string& shaderPath);
        ~VulkanShader();

    private:
        void CreateShaderModule(const std::vector<char>& shaderCode);

    private:
        VkShaderModule      m_ShaderModule;
        VulkanGlobalContext m_GlobalContext;
    };
} // namespace Galaxy