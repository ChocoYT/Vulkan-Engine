cmake_minimum_required(VERSION 3.13)
project(Vulkan-Engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Initialize Directories
set(SOURCE_DIRECTORY   ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIRECTORY  ${PROJECT_SOURCE_DIR}/include)
set(EXTERNAL_DIRECTORY ${PROJECT_SOURCE_DIR}/external)
set(SHADER_DIRECTORY   ${PROJECT_SOURCE_DIR}/shaders)
set(INSTALL_DIRECTORY  ${PROJECT_SOURCE_DIR}/install)

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

# Link Libraries
find_package(Vulkan REQUIRED)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    glm
)

# Compile Shaders
set(SHADER_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders)
set(SPIRV_OUTPUT_DIR ${INSTALL_DIRECTORY}/$<CONFIG>/shaders)

file(GLOB HLSL_SHADERS ${SHADER_DIRECTORY}/*.hlsl)

set(SPIRV_SHADERS "")

foreach(SHADER ${HLSL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

    if(SHADER_NAME MATCHES ".*VS.*")
        set(TARGET_PROFILE "vs_6_0")
    elseif(SHADER_NAME MATCHES ".*PS.*")
        set(TARGET_PROFILE "ps_6_0")
    else()
        message(FATAL_ERROR "Unknown shader stage for ${SHADER_NAME}, must contain VS or PS")
    endif()
    
    set(OUT_FILE "${SPIRV_OUTPUT_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${OUT_FILE}
        COMMAND $ENV{VK_SDK_PATH}/Bin/dxc.exe -T ${TARGET_PROFILE} -E main -Fo ${OUT_FILE} ${SHADER} -spirv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V (${TARGET_PROFILE})"
        VERBATIM
    )

    list(APPEND SPIRV_SHADERS ${OUT_FILE})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SPIRV_SHADERS})
