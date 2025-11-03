cmake_minimum_required(VERSION 3.26)
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

file(MAKE_DIRECTORY ${INSTALL_DIRECTORY})

# Sources
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    ${SOURCE_DIRECTORY}/*.c
    ${SOURCE_DIRECTORY}/*.cpp
)

# Headers
file(GLOB_RECURSE HEADERS
    CONFIGURE_DEPENDS
    ${INCLUDE_DIRECTORY}/*.h
    ${INCLUDE_DIRECTORY}/*.hpp
)

# Add Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORY})

# Link Libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install path" FORCE)

find_package(Vulkan REQUIRED)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/assimp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    assimp
)

# Runtime Output
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${INSTALL_DIRECTORY}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${INSTALL_DIRECTORY}/Release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${INSTALL_DIRECTORY}/RelWithDebInfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${INSTALL_DIRECTORY}/MinSizeRel"
)

# Assets and Shaders
set(ASSET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/assets)
set(ASSET_BUILD_DIR  ${INSTALL_DIRECTORY}/$<CONFIG>/assets)

set(SHADER_SOURCE_DIR ${ASSET_SOURCE_DIR}/shaders)
set(SHADER_BUILD_DIR  ${ASSET_BUILD_DIR}/shaders)

# Glob HLSL Shaders
file(GLOB_RECURSE HLSL_SHADERS
    CONFIGURE_DEPENDS
    ${SHADER_SOURCE_DIR}/*.hlsl
)

set(SPIRV_SHADERS "")

if(NOT DEFINED ENV{VK_SDK_PATH})
    message(WARNING "VK_SDK_PATH not Set. Shader Compilation may Fail.")
endif()

# Compile HLSL Shaders
foreach(SHADER ${HLSL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

    if(SHADER_NAME MATCHES ".*VS.*")
        set(TARGET_PROFILE "vs_6_0")
    elseif(SHADER_NAME MATCHES ".*PS.*")
        set(TARGET_PROFILE "ps_6_0")
    else()
        message(FATAL_ERROR "Unknown shader stage for ${SHADER_NAME}, must contain VS or PS")
    endif()

    set(OUT_FILE ${SHADER_BUILD_DIR}/${SHADER_NAME}.spv)

    add_custom_command(
        OUTPUT ${OUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BUILD_DIR}
        COMMAND $ENV{VK_SDK_PATH}/Bin/dxc.exe -T ${TARGET_PROFILE} -E main -Fo ${OUT_FILE} ${SHADER} -spirv
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V (${TARGET_PROFILE})"
        VERBATIM
    )

    list(APPEND SPIRV_SHADERS "${OUT_FILE}")
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SPIRV_SHADERS})

# Copy Assets
install(
    DIRECTORY ${ASSET_SOURCE_DIR}/models
    DESTINATION ${ASSET_BUILD_DIR}/models
)
install(
    DIRECTORY ${ASSET_SOURCE_DIR}/textures
    DESTINATION ${ASSET_BUILD_DIR}/textures
)
