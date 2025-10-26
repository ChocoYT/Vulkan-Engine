#include "Vulkan/Buffers/Staging.hpp"

#include "Vulkan/Resources/Buffer.hpp"

StagingBuffer::StagingBuffer(
    const Context     &context,
    const CommandPool &commandPool,
    MemoryAllocator   &allocator
) : m_context(context), m_commandPool(commandPool), m_allocator(allocator) {}

StagingBuffer::~StagingBuffer()
{
    cleanup();
}

void StagingBuffer::init(VkDeviceSize bufferSize)
{
    m_bufferSize = bufferSize;

    // Initialize Staging Buffer
    m_buffer = std::make_unique<Buffer>(m_context, m_commandPool, m_allocator);
    m_buffer->init(
        m_bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
}

void StagingBuffer::cleanup()
{
    m_buffer->cleanup();
}

void StagingBuffer::update(void *bufferData)
{
    m_buffer->update(bufferData, m_bufferSize);
}

void StagingBuffer::copyTo(const Buffer &dst)
{
    m_buffer->copyTo(dst);
}
