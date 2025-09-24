cmake_minimum_required(VERSION 3.13)
project(Vulkan-Engine)
set(CMAKE_CXX_STANDARD 17)

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
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

# GLFW
add_subdirectory(external/GLFW)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE external/GLFW/include)
