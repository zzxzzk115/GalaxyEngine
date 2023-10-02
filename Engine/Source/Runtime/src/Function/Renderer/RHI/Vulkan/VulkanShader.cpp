#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanShader.h"
#include "GalaxyEngine/Core/FileSystem.h"
#include "GalaxyEngine/Function/Global/GlobalContext.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGlobalContext.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"
#include "GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanMacro.h"

namespace Galaxy
{
    VulkanShader::VulkanShader(const std::string& shaderPath)
    {
        auto shaderCode = g_RuntimeGlobalContext.FileSys->ReadFileAllText(shaderPath);
        CreateShaderModule(shaderCode);
    }

    VulkanShader::~VulkanShader() { vkDestroyShaderModule(g_VulkanGlobalContext.Device, m_ShaderModule, nullptr); }

    void VulkanShader::CreateShaderModule(const std::vector<char>& shaderCode)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize                 = shaderCode.size();
        createInfo.pCode                    = reinterpret_cast<const uint32_t*>(shaderCode.data());

        auto result = vkCreateShaderModule(g_VulkanGlobalContext.Device, &createInfo, nullptr, &m_ShaderModule);
        VK_CHECK(result, "Failed to create shader module!");
    }
} // namespace Galaxy