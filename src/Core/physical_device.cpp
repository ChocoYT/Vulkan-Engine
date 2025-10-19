#include "Core/physical_device.hpp"

void PhysicalDevice::init(
    VkInstance   instance,
    VkSurfaceKHR surface
) {
    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0 || result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEnumeratePhysicalDevices' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto &deviceCandidate : devices)
    {
        if (PhysicalDevice::isDeviceSuitable(surface, deviceCandidate))
        {
            m_handle = deviceCandidate;
            break;
        }
    }

    if (m_handle == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a Suitable GPU.");
    
    std::cout << "[INFO]\tPhysical Device Selected Successfully.\n";
}

bool PhysicalDevice::isDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device)
{
    findQueueFamilies(surface, device);

    return m_graphicsQueueFamily != UINT32_MAX && m_presentQueueFamily != UINT32_MAX;
}

void PhysicalDevice::findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilies.size(); ++i)
    {
        const auto &queueFamily = queueFamilies[i];
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            m_graphicsQueueFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
            m_presentQueueFamily = i;

        if (m_graphicsQueueFamily != UINT32_MAX && m_presentQueueFamily != UINT32_MAX)
            break;
    }
}
