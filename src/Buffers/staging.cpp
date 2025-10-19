#include "Buffers/staging.hpp"

#include "Buffers/buffer.hpp"

StagingBuffer::StagingBuffer(Context &context) : m_context(context) {}
StagingBuffer::~StagingBuffer()
{
    cleanup();
}

void StagingBuffer::init(VkDeviceSize bufferSize)
{
    m_bufferSize = bufferSize;

    // Initialize Staging Buffer
    m_buffer = std::make_unique<Buffer>(m_context);
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
