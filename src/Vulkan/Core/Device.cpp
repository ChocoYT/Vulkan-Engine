#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"

Device::Device(const Context &context) : m_context(context) {}
Device::~Device()
{
    cleanup();
}

void Device::init()
{
    VkPhysicalDevice physicalDevice = m_context.getPhysicalDevice().getHandle();
    
    uint32_t graphicsQueueFamily = m_context.getPhysicalDevice().getGraphicsQueueFamily();
    uint32_t presentQueueFamily  = m_context.getPhysicalDevice().getPresentQueueFamily();

    VkResult result = VK_SUCCESS;

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
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

    result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDevice' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Logical Device.");
    }
    
    // Get Queue Handles
    vkGetDeviceQueue(m_handle, graphicsQueueFamily, 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_handle, presentQueueFamily, 0, &m_presentQueue);

    std::cout << "[INFO]\tLogical Device Created Successfully.\n";
}

void Device::cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyDevice(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
