#include "Vulkan/Core/Surface.hpp"

#include "window.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Instance.hpp"

Surface::Surface(
    const Window  &window,
    const Context &context
) : m_window(window), m_context(context) {}

Surface::~Surface()
{
    cleanup();
}

void Surface::init()
{
    GLFWwindow *window  = m_window.getHandle();
    VkInstance instance = m_context.getInstance().getHandle();

    VkResult result = VK_SUCCESS;
    
    result = glfwCreateWindowSurface(instance, window, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'glfwCreateWindowSurface' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to create Vulkan Surface.");
    }
    
    std::cout << "[INFO]\tVulkan Surface Created Successfully.\n";
}

void Surface::cleanup()
{
    VkInstance instance = m_context.getInstance().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}