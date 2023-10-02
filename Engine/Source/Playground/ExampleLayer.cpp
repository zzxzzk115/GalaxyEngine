#include "ExampleLayer.h"

#include <GalaxyEngine/Core/Macro.h>
#include <GalaxyEngine/Function/Renderer/RHI/Vulkan/VulkanShader.h>
#include <GalaxyEngine/Function/Global/GlobalContext.h>

// TODO: Remove, test only!
#include <vulkan/vulkan.h>

ExampleLayer::ExampleLayer() : Galaxy::Layer("ExampleLayer") {}

void ExampleLayer::OnAttach() 
{ 
	GAL_INFO("Playground Example Layer OnAttach...");

    // Vulkan has already initialized here.
	
	// Triangle Demo without abstraction

	// 1. Setup Shader Stages
    auto vertextShaderModule  = Galaxy::VulkanShader("Resources/Shaders/spv/triangle.vert.spv");
    auto fragmentShaderModule = Galaxy::VulkanShader("Resources/Shaders/spv/triangle.frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module                          = vertextShaderModule.GetModule();
    vertShaderStageInfo.pName                           = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    vertShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
    vertShaderStageInfo.module                          = fragmentShaderModule.GetModule();
    vertShaderStageInfo.pName                           = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // 2. Setup Fixed Functions

    // 3. Setup Pipeline Layout

    // 4. Setup Pass & Subpasses

    // 5. Create Vulkan Graphics Pipeline

    // 6. Create Framebuffers

    // 7. Create Command Pool

    // 8. Render and Present!

    // 9. Recreate Swapchain
}

void ExampleLayer::OnDetach() { GAL_INFO("Playground Example Layer OnDetach..."); }

void ExampleLayer::OnUpdate(Galaxy::TimeStep ts) {}

void ExampleLayer::OnEvent(Galaxy::Event& e) {}