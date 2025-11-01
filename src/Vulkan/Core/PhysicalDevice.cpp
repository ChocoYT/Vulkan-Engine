#include "Vulkan/Core/PhysicalDevice.hpp"

#include <iostream>
#include <vector>

#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/Surface.hpp"

VulkanPhysicalDevice::VulkanPhysicalDevice(
    VkPhysicalDevice handle,
    uint32_t         graphicsQueueFamily,
    uint32_t         presentQueueFamily
) : m_handle(handle),
    m_graphicsQueueFamily(graphicsQueueFamily),
    m_presentQueueFamily(presentQueueFamily)
{}

VulkanPhysicalDevice::~VulkanPhysicalDevice() = default;

std::unique_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Create(
    const VulkanInstance &instance,
    const VulkanSurface  &surface
) {
    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;
    
    result = vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, nullptr);
    if (deviceCount == 0 || result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEnumeratePhysicalDevices' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.GetHandle(), &deviceCount, devices.data());

    VkPhysicalDevice handle              = VK_NULL_HANDLE;
    uint32_t         graphicsQueueFamily = VK_QUEUE_FAMILY_IGNORED;
    uint32_t         presentQueueFamily  = VK_QUEUE_FAMILY_IGNORED;

    for (const auto &deviceCandidate : devices)
    {
        QueueFamilyIndices indices = VulkanPhysicalDevice::FindQueueFamilies(surface.GetHandle(), deviceCandidate);
        if (indices.isComplete())
        {
            handle              = deviceCandidate;
            graphicsQueueFamily = indices.graphicsFamily.value();
            presentQueueFamily  = indices.presentFamily.value();
            break;
        }
    }

    if (handle == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a Suitable GPU.");
    
    std::cout << "[INFO]\tPhysical Device Selected Successfully.\n";

    return std::unique_ptr<VulkanPhysicalDevice>(
        new VulkanPhysicalDevice(
            handle,
            graphicsQueueFamily,
            presentQueueFamily
        )
    );
}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkSurfaceKHR vkSurface, VkPhysicalDevice vkPhysicalDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(vkSurface, vkPhysicalDevice);

    return indices.isComplete();
}

QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkSurfaceKHR vkSurface, VkPhysicalDevice vkPhysicalDevice)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndices indices;
    for (uint32_t i = 0; i < queueFamilies.size(); ++i)
    {
        const auto &queueFamily = queueFamilies[i];
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentSupport);
        
        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isComplete())
            break;
    }

    return indices;
}

VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanPhysicalDevice &&other) noexcept : 
    m_handle(other.m_handle),
    m_graphicsQueueFamily(other.m_graphicsQueueFamily),
    m_presentQueueFamily(other.m_presentQueueFamily)
{
    other = VulkanPhysicalDevice{};
}

VulkanPhysicalDevice& VulkanPhysicalDevice::operator=(VulkanPhysicalDevice &&other) noexcept
{
    if (this != &other)
    {
        m_handle              = other.m_handle;
        m_graphicsQueueFamily = other.m_graphicsQueueFamily;
        m_presentQueueFamily  = other.m_presentQueueFamily;

        other = VulkanPhysicalDevice{};
    }

    return *this;
}
