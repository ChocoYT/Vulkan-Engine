#include "Buffers/index.hpp"

#include "Buffers/buffer.hpp"
#include "Buffers/staging.hpp"

IndexBuffer::IndexBuffer(Context &context) : m_context(context) {}
IndexBuffer::~IndexBuffer()
{
    cleanup();
}

void IndexBuffer::init(
    VkDeviceSize bufferSize,
    uint32_t     bufferCount
) {
    m_bufferSize  = bufferSize;
    m_bufferCount = bufferCount;

    // Initialize Index Buffers
    for (uint32_t i = 0; i < bufferCount; ++i)
    {
        m_buffers.emplace_back(std::make_unique<Buffer>(m_context));
        m_buffers.back()->init(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
    }

    // Initialize Staging Buffers
    for (uint32_t i = 0; i < bufferCount; ++i)
    {
        m_stagingBuffers.emplace_back(std::make_unique<StagingBuffer>(m_context));
        m_stagingBuffers.back()->init(bufferSize);
    }
}

void IndexBuffer::cleanup()
{
    for (auto& buffer : m_buffers) {
        if (buffer) buffer->cleanup();
    }
    for (auto& stagingBuffer : m_stagingBuffers) {
        if (stagingBuffer) stagingBuffer->cleanup();
    }
    
    m_buffers.clear();
    m_stagingBuffers.clear();
}

void IndexBuffer::bind(
    VkCommandBuffer commandBuffer,
    uint32_t        currentFrame
) {
    vkCmdBindIndexBuffer(
        commandBuffer,
        m_buffers[currentFrame]->getHandle(),
        0,
        VK_INDEX_TYPE_UINT32
    );
}

void IndexBuffer::update(
    void     *bufferData,
    uint32_t currentFrame
) {
    m_stagingBuffers[currentFrame]->update(bufferData);
    m_stagingBuffers[currentFrame]->copyTo(*m_buffers[currentFrame]);
}
