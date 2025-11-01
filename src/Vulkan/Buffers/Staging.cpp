#include "Vulkan/Buffers/Staging.hpp"

#include "Vulkan/Resources/Buffer.hpp"

VulkanStagingBuffer::VulkanStagingBuffer(
    std::unique_ptr<VulkanBuffer> buffer,
    VkDeviceSize size
) : m_buffer(std::move(buffer)),
    m_size(size)
{}

VulkanStagingBuffer::~VulkanStagingBuffer() = default;

std::unique_ptr<VulkanStagingBuffer> VulkanStagingBuffer::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize size
) {
    // Initialize Staging Buffer
    auto buffer = VulkanBuffer::Create(
        physicalDevice,
        device,
        allocator,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    return std::unique_ptr<VulkanStagingBuffer>(
        new VulkanStagingBuffer(
            std::move(buffer),
            size
        )
    );
}

void VulkanStagingBuffer::Update(void *data)
{
    m_buffer->Update(data, m_size);
}

void VulkanStagingBuffer::CopyTo(
    const VulkanCommandPool &commandPool,
    const VulkanBuffer      &dst
) {
    m_buffer->CopyTo(commandPool, dst);
}

VulkanStagingBuffer::VulkanStagingBuffer(VulkanStagingBuffer&& other) noexcept : 
    m_size(other.m_size),
    m_buffer(std::move(other.m_buffer))
{
    other = VulkanStagingBuffer{};
}

VulkanStagingBuffer& VulkanStagingBuffer::operator=(VulkanStagingBuffer &&other) noexcept
{
    if (this != &other)
    {
        m_size   = other.m_size;
        m_buffer = std::move(other.m_buffer);

        other = VulkanStagingBuffer{};
    }
    return *this;
}
