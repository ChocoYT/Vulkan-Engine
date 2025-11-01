#include "Vulkan/Buffers/Vertex.hpp"

#include "Vulkan/Resources/Buffer.hpp"
#include "Vulkan/Buffers/Staging.hpp"

VulkanVertexBuffer::VulkanVertexBuffer(
    std::vector<std::unique_ptr<VulkanBuffer>>        buffers,
    std::vector<std::unique_ptr<VulkanStagingBuffer>> stagingBuffers,
    VkDeviceSize size,
    uint32_t     count
) : m_buffers(std::move(buffers)),
    m_stagingBuffers(std::move(stagingBuffers)),
    m_size(size),
    m_count(count)
{}

VulkanVertexBuffer::~VulkanVertexBuffer() = default;

std::unique_ptr<VulkanVertexBuffer> VulkanVertexBuffer::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize size,
    uint32_t     count
) {
    // Initialize Vertex Buffers
    std::vector<std::unique_ptr<VulkanBuffer>> buffers;

    for (uint32_t i = 0; i < count; ++i)
    {
        buffers.emplace_back(
            VulkanBuffer::Create(
                physicalDevice,
                device,
                allocator,
                size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
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

    return std::unique_ptr<VulkanVertexBuffer>(
        new VulkanVertexBuffer(
            std::move(buffers),
            std::move(stagingBuffers),
            size,
            count
        )
    );
}

void VulkanVertexBuffer::Cleanup()
{   
    m_buffers.clear();
    m_stagingBuffers.clear();
}

void VulkanVertexBuffer::Bind(
    VkCommandBuffer vkCommandBuffer,
    uint32_t        currentFrame
) {
    VkBuffer     buffers[] = { m_buffers[currentFrame]->GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(
        vkCommandBuffer,
        0,
        1,
        buffers,
        offsets
    );
}

void VulkanVertexBuffer::Update(
    const VulkanCommandPool &commandPool,
    void     *data,
    uint32_t currentFrame
) {
    m_stagingBuffers[currentFrame]->Update(data);
    m_stagingBuffers[currentFrame]->CopyTo(commandPool, *m_buffers[currentFrame]);
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanVertexBuffer&& other) noexcept : 
    m_buffers(std::move(other.m_buffers)),
    m_stagingBuffers(std::move(other.m_stagingBuffers)),
    m_size(other.m_size),
    m_count(other.m_count)
{
    other = VulkanVertexBuffer{};
}

VulkanVertexBuffer& VulkanVertexBuffer::operator=(VulkanVertexBuffer &&other) noexcept
{
    if (this != &other)
    {
        Cleanup(); 

        m_buffers        = std::move(other.m_buffers);
        m_stagingBuffers = std::move(other.m_stagingBuffers);
        m_size           = other.m_size;
        m_count          = other.m_count;

        other = VulkanVertexBuffer{};
    }

    return *this;
}
