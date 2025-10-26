#include "Vulkan/Descriptors/DescriptorPool.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

DescriptorPool::DescriptorPool(const Context &context) : m_context(context) {}
DescriptorPool::~DescriptorPool()
{
    cleanup();
}

void DescriptorPool::init(uint32_t frameCount)
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorPoolSize> poolSizes(1);
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = frameCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = frameCount;

    result = vkCreateDescriptorPool(vkDevice, &poolInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Pool.");
    }

    std::cout << "[INFO]\tDescriptor Pool Created Successfully.\n";
}

void DescriptorPool::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}