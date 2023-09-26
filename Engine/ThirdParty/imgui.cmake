set(TARGET_NAME imgui)
set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS "${IMGUI_SOURCE_DIR}/*.cpp")

file(GLOB imgui_impl CONFIGURE_DEPENDS
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
            "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.h")

find_package(Vulkan REQUIRED)

add_library(${TARGET_NAME} STATIC ${imgui_sources} ${imgui_impl})

target_include_directories(${TARGET_NAME} 
    PUBLIC ${Vulkan_INCLUDE_DIRS}
    PUBLIC $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC ${Vulkan_LIBRARIES})