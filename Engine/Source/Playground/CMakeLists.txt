set(TARGET_NAME "GalaxyPlayground")
set(TARGET_BINARY_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME})

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_executable(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Test")

# Set output path
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_BINARY_DIR})
if (MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TARGET_BINARY_DIR})
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TARGET_BINARY_DIR})
endif ()

# Platform
# if (PLATFORM_WINDOWS)
#     if (MSVC)
#         if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#             set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
#         else ()
#             set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#         endif ()
#     endif ()
# endif ()

target_link_libraries(${TARGET_NAME} GalaxyRuntime)