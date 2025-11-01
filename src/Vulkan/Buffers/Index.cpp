#include "Vulkan/Buffers/Index.hpp"

#include "Vulkan/Resources/Buffer.hpp"
#include "Vulkan/Buffers/Staging.hpp"

VulkanIndexBuffer::VulkanIndexBuffer(
    std::vector<std::unique_ptr<VulkanBuffer>>        buffers,
    std::vector<std::unique_ptr<VulkanStagingBuffer>> stagingBuffers,
    VkDeviceSize size,
    uint32_t     count
) : m_buffers(std::move(buffers)),
    m_stagingBuffers(std::move(stagingBuffers)),
    m_size(size),
    m_count(count)
{}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    Cleanup();
}

std::unique_ptr<VulkanIndexBuffer> VulkanIndexBuffer::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize size,
    uint32_t     count
) {
    // Initialize Index Buffers
    std::vector<std::unique_ptr<VulkanBuffer>> buffers;

    for (uint32_t i = 0; i < count; ++i)
    {
        buffers.emplace_back(
            VulkanBuffer::Create(
                physicalDevice,
                device,
                allocator,
                size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            )
        );
    }

    // Initialize Staging Buffers
    std::vector<std::unique_ptr<VulkanStagingBuffer>> stagingBuffers;

    for (uint32_t i = 0; i < count; ++i)
    {
        stagingBuffers.emplace_back(
            VulkanStagingBuffer::Create(
                physicalDevice,
                device,
                allocator,
                size
            )
        );
    }

    return std::unique_ptr<VulkanIndexBuffer>(
        new VulkanIndexBuffer(
            std::move(buffers),
            std::move(stagingBuffers),
            size,
            count
        )
    );
}

void VulkanIndexBuffer::Cleanup()
{
    m_buffers.clear();
    m_stagingBuffers.clear();
}

void VulkanIndexBuffer::Bind(
    VkCommandBuffer vkCommandBuffer,
    uint32_t        currentFrame
) {
    vkCmdBindIndexBuffer(
        vkCommandBuffer,
        m_buffers[currentFrame]->GetHandle(),
        0,
        VK_INDEX_TYPE_UINT32
    );
}

void VulkanIndexBuffer::Update(
    const VulkanCommandPool &commandPool,
    void     *data,
    uint32_t currentFrame
) {
    m_stagingBuffers[currentFrame]->Update(data);
    m_stagingBuffers[currentFrame]->CopyTo(commandPool, *m_buffers[currentFrame]);
}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanIndexBuffer&& other) noexcept : 
    m_buffers(std::move(other.m_buffers)),
    m_stagingBuffers(std::move(other.m_stagingBuffers)),
    m_size(other.m_size),
    m_count(other.m_count)
{
    other = VulkanIndexBuffer{};
}

VulkanIndexBuffer& VulkanIndexBuffer::operator=(VulkanIndexBuffer &&other) noexcept
{
    if (this != &other)
    {
        Cleanup(); 

        m_buffers        = std::move(other.m_buffers);
        m_stagingBuffers = std::move(other.m_stagingBuffers);
        m_size           = other.m_size;
        m_count          = other.m_count;

        other = VulkanIndexBuffer{};
    }
    
    return *this;
}
