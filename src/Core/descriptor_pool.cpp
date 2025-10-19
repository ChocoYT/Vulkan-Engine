#include "Core/descriptor_pool.hpp"

void DescriptorPool::init(
    VkDevice device,
    
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

    result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Pool.");
    }

    std::cout << "[INFO]\tDescriptor Pool Created Successfully.\n";
}

void DescriptorPool::cleanup(VkDevice device)
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}