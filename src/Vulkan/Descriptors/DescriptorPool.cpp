#include "Vulkan/Descriptors/DescriptorPool.hpp"

#include <iostream>
#include <vector>

#include "Vulkan/Core/Device.hpp"

VulkanDescriptorPool::VulkanDescriptorPool(
    const VulkanDevice &device,
    VkDescriptorPool handle
) : m_device(device),
    m_handle(handle)
{}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    Cleanup();
}

std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Create(
    const VulkanDevice &device,
    uint32_t frameCount
) {
    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorPoolSize> poolSizes(1);
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = frameCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = frameCount;

    VkDescriptorPool handle = VK_NULL_HANDLE;
    result = vkCreateDescriptorPool(device.GetHandle(), &poolInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Pool.");
    }

    std::cout << "[INFO]\tDescriptor Pool Created Successfully.\n";

    return std::unique_ptr<VulkanDescriptorPool>(
        new VulkanDescriptorPool(
            device,
            handle
        )
    );
}

void VulkanDescriptorPool::Cleanup()
{
    // Destroy Descriptor Pool
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDescriptorPool &&other) noexcept :
    m_device(other.m_device),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanDescriptorPool& VulkanDescriptorPool::operator=(VulkanDescriptorPool &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}
