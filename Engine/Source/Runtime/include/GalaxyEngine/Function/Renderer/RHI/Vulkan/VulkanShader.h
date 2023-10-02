#pragma once

#include "GalaxyEngine/Core/Base.h"
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
    };
} // namespace Galaxy