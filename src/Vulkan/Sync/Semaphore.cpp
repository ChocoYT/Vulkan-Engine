#include "Vulkan/Sync/Semaphore.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"

VulkanSemaphore::VulkanSemaphore(
    const VulkanDevice &device,
    VkSemaphore     handle,
    VkSemaphoreType type
) : m_device(device),
    m_handle(handle),
    m_type(type)
{}

VulkanSemaphore::~VulkanSemaphore()
{
    Cleanup();
}

std::unique_ptr<VulkanSemaphore> VulkanSemaphore::Create(
    const VulkanDevice &device,
    VkSemaphoreType type
) {
    VkResult result = VK_SUCCESS;

    VkSemaphoreTypeCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.semaphoreType = type;
    semaphoreCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &semaphoreCreateInfo;
    createInfo.flags = 0;

    // Create Semaphore
    VkSemaphore handle = VK_NULL_HANDLE;
    result = vkCreateSemaphore(device.GetHandle(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSemaphore' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Semaphore.");
    }

    std::cout << "[INFO]\tSemaphore Created Successfully.\n";

    return std::unique_ptr<VulkanSemaphore>(
        new VulkanSemaphore(
            device,
            handle,
            type
        )
    );
}

void VulkanSemaphore::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanSemaphore::VulkanSemaphore(VulkanSemaphore &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle),
    m_type(other.m_type)
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_type   = VK_SEMAPHORE_TYPE_BINARY;
}

VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;
        m_type   = other.m_type;

        other.m_handle = VK_NULL_HANDLE;
        other.m_type   = VK_SEMAPHORE_TYPE_BINARY;
    }

    return *this;
}
