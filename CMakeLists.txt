cmake_minimum_required(VERSION 3.13)
project(Vulkan-Engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Initialize Directories
set(SOURCE_DIRECTORY  ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set(INSTALL_DIRECTORY ${PROJECT_SOURCE_DIR}/install)

# ----------
# Find Files
# ----------

# Sources
file(GLOB_RECURSE SOURCES
    ${SOURCE_DIRECTORY}/*.c
    ${SOURCE_DIRECTORY}/*.cpp
)
# Headers
file(GLOB_RECURSE HEADERS
    ${INCLUDE_DIRECTORY}/*.h
    ${INCLUDE_DIRECTORY}/*.hpp
)

# Install Directory
file(MAKE_DIRECTORY ${INSTALL_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${INSTALL_DIRECTORY})

# Add Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORY})

# --------------
# Link Libraries
# --------------

# Vulkan
find_package (Vulkan REQUIRED)

add_library(VulkanCppModule)
add_library(Vulkan::cppm ALIAS VulkanCppModule)

target_compile_definitions(VulkanCppModule PUBLIC
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)
target_include_directories(VulkanCppModule PRIVATE ${Vulkan_INCLUDE_DIR})

target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)
set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

target_sources(VulkanCppModule
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
    ${Vulkan_INCLUDE_DIR}
    FILES
    ${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm
)
target_link_libraries(${PROJECT_NAME} Vulkan::cppm)

# GLFW
add_subdirectory(external/GLFW)
target_link_libraries(${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PRIVATE external/GLFW/include)

# GLM
target_include_directories(${PROJECT_NAME} PRIVATE external/GLM/include)

# ----------
# Copy Files
# ----------

# Shaders
set(SHADER_DIRECTORY ${PROJECT_SOURCE_DIR}/shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_DIRECTORY}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)
