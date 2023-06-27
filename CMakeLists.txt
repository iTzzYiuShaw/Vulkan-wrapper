cmake_minimum_required(VERSION 3.24)
project(IndividualProject)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIRSRCS)

include_directories(
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/glm/include
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/glfw/include
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/Vulkan/Include
)

link_directories(
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/glm/lib
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/glfw/lib
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/Vulkan/Lib
)

add_subdirectory(vulkanWrapper)

add_executable(IndividualProject ${DIRSRCS} model.h)

target_link_libraries(IndividualProject vulkanLib vulkan-1.lib glfw3.lib)