#include "Function/Renderer/RHI/Vulkan/VulkanShader.h"
#include "Core/FileSystem.h"
#include "Function/Renderer/RHI/Vulkan/VulkanGraphicsContext.h"
#include "Function/Renderer/RHI/Vulkan/VulkanMacro.h"

namespace Galaxy
{
    VulkanShader::VulkanShader(const std::string& shaderPath) :
        m_GlobalContext(VulkanGraphicsContext::GetGlobalContext())
    {
        auto shaderCode = FileSystem::ReadFileAllText(shaderPath);
        CreateShaderModule(shaderCode);
    }

    VulkanShader::~VulkanShader() { vkDestroyShaderModule(m_GlobalContext.Device, m_ShaderModule, nullptr); }

    void VulkanShader::CreateShaderModule(const std::vector<char>& shaderCode)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize                 = shaderCode.size();
        createInfo.pCode                    = reinterpret_cast<const uint32_t*>(shaderCode.data());

        auto result = vkCreateShaderModule(m_GlobalContext.Device, &createInfo, nullptr, &m_ShaderModule);
        VK_CHECK(result, "Failed to create shader module!");
    }
} // namespace Galaxy