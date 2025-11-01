#include "Vulkan/Core/Device.hpp"

#include <iostream>
#include <set>
#include <vector>

#include "Vulkan/Core/PhysicalDevice.hpp"

VulkanDevice::VulkanDevice(
    VkDevice handle,
    VkQueue  graphicsQueue,
    VkQueue  presentQueue,
    uint32_t graphicsQueueFamily,
    uint32_t presentQueueFamily
) : m_handle(handle),
    m_graphicsQueue(graphicsQueue),
    m_presentQueue(presentQueue),
    m_graphicsQueueFamily(graphicsQueueFamily),
    m_presentQueueFamily(presentQueueFamily)
{}

VulkanDevice::~VulkanDevice()
{
    Cleanup();
}

std::unique_ptr<VulkanDevice> VulkanDevice::Create(
    const VulkanPhysicalDevice &physicalDevice
) {
    VkResult result = VK_SUCCESS;

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        physicalDevice.GetGraphicsQueueFamily(),
        physicalDevice.GetPresentQueueFamily()
    };

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Features
    VkPhysicalDeviceFeatures deviceFeatures{};

    // Extensions
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Create Info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos    = queueCreateInfos.data();
    createInfo.pEnabledFeatures     = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Create Logical Device
    VkDevice handle;
    result = vkCreateDevice(physicalDevice.GetHandle(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDevice' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Logical Device.");
    }
    
    // Get Queue Handles
    VkQueue graphicsQueue = 0;
    VkQueue presentQueue  = 0;
    vkGetDeviceQueue(handle, physicalDevice.GetGraphicsQueueFamily(), 0, &graphicsQueue);
    vkGetDeviceQueue(handle, physicalDevice.GetPresentQueueFamily(), 0, &presentQueue);

    std::cout << "[INFO]\tLogical Device Created Successfully.\n";

    return std::unique_ptr<VulkanDevice>(
        new VulkanDevice(
            handle,
            graphicsQueue,
            presentQueue,
            physicalDevice.GetGraphicsQueueFamily(),
            physicalDevice.GetPresentQueueFamily()
        )
    );
}

void VulkanDevice::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanDevice::VulkanDevice(VulkanDevice &&other) noexcept : 
    m_handle(other.m_handle),
    m_graphicsQueue(other.m_graphicsQueue),
    m_presentQueue(other.m_presentQueue),
    m_graphicsQueueFamily(other.m_graphicsQueueFamily),
    m_presentQueueFamily(other.m_presentQueueFamily)
{
    other = VulkanDevice{};
}

VulkanDevice& VulkanDevice::operator=(VulkanDevice &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle              = other.m_handle;
        m_graphicsQueue       = other.m_graphicsQueue;
        m_presentQueue        = other.m_presentQueue;
        m_graphicsQueueFamily = other.m_graphicsQueueFamily;
        m_presentQueueFamily  = other.m_presentQueueFamily;

        other = VulkanDevice{};
    }

    return *this;
}
