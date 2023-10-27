set(ENGINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/ThirdParty")
set(ENGINE_SRC_DIR "${ENGINE_ROOT_DIR}/Source")

set(GLFW_INCLUDE_DIR "${THIRD_PARTY_DIR}/glfw/include")
set(IMGUI_INCLUDE_DIR "${THIRD_PARTY_DIR}/imgui")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GalaxyEditor)
endif()

find_package(Vulkan REQUIRED)

# Compile Shader
set(glslangValidator_executable glslangValidator)
set(SHADER_COMPILE_TARGET GalaxyShaderCompile)
add_subdirectory(Shaders)

add_subdirectory(ThirdParty)
add_subdirectory(Source/Runtime)
add_subdirectory(Source/Playground)