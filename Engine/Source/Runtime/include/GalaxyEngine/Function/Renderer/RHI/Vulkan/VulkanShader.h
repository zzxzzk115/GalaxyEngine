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

        VkShaderModule GetModule() const { return m_ShaderModule; }

    private:
        void CreateShaderModule(const std::vector<char>& shaderCode);

    private:
        VkShaderModule      m_ShaderModule;
    };
} // namespace Galaxy