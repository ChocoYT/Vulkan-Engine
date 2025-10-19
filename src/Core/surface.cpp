#include "Core/surface.hpp"

void Surface::init(
    VkInstance  instance,
    GLFWwindow *window
) {
    VkResult result = VK_SUCCESS;
    
    result = glfwCreateWindowSurface(instance, window, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'glfwCreateWindowSurface' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to create Vulkan Surface.");
    }
    
    std::cout << "[INFO]\tVulkan Surface Created Successfully.\n";
}

void Surface::cleanup(VkInstance instance)
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}