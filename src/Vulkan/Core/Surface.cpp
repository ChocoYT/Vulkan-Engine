#include "Vulkan/Core/Surface.hpp"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Window/Window.hpp"
#include "Vulkan/Core/Instance.hpp"

VulkanSurface::VulkanSurface(
    const VulkanInstance &instance,
    VkSurfaceKHR         handle
) : m_instance(instance),
    m_handle(handle)
{}

VulkanSurface::~VulkanSurface()
{
    Cleanup();
}

std::unique_ptr<VulkanSurface> VulkanSurface::Create(
    const Window         &window,
    const VulkanInstance &instance
) {
    VkResult result = VK_SUCCESS;
    
    // Create Surface
    VkSurfaceKHR handle = VK_NULL_HANDLE;
    result = glfwCreateWindowSurface(instance.GetHandle(), window.GetHandle(), nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'glfwCreateWindowSurface' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to create Vulkan Surface.");
    }
    
    std::cout << "[INFO]\tVulkan Surface Created Successfully.\n";
    
    return std::unique_ptr<VulkanSurface>(
        new VulkanSurface(
            instance,
            handle
        )
    );
}

void VulkanSurface::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanSurface::VulkanSurface(VulkanSurface &&other) noexcept : 
    m_instance(other.m_instance),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanSurface& VulkanSurface::operator=(VulkanSurface &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}