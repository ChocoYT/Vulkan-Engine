#include "Vulkan/Sync/Fence.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"

VulkanFence::VulkanFence(
    const VulkanDevice &device,
    VkFence            handle,
    VkFenceCreateFlags flags
) : m_device(device),
    m_handle(handle),
    m_flags(flags)
{}

VulkanFence::~VulkanFence()
{
    Cleanup();
}

std::unique_ptr<VulkanFence> VulkanFence::Create(
    const VulkanDevice &device,
    VkFenceCreateFlags flags
) {
    VkResult result = VK_SUCCESS;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;

    // Create Fence
    VkFence handle = VK_NULL_HANDLE;
    result = vkCreateFence(device.GetHandle(), &fenceInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFence' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Fence.");
    }

    std::cout << "[INFO]\tFence Created Successfully.\n";

    return std::unique_ptr<VulkanFence>(
        new VulkanFence(
            device,
            handle,
            flags
        )
    );
}

void VulkanFence::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyFence(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void VulkanFence::Wait()
{
    vkWaitForFences(m_device.GetHandle(), 1, &m_handle, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.GetHandle(), 1, &m_handle);
}

VulkanFence::VulkanFence(VulkanFence &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle),
    m_flags(other.m_flags)
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_flags  = 0;
}

VulkanFence& VulkanFence::operator=(VulkanFence &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;
        m_flags  = other.m_flags;

        other.m_handle = VK_NULL_HANDLE;
        other.m_flags  = 0;
    }

    return *this;
}