#include "Buffers/vertex.hpp"

#include "Buffers/buffer.hpp"
#include "Buffers/staging.hpp"

VertexBuffer::VertexBuffer(Context &context) : m_context(context) {}
VertexBuffer::~VertexBuffer()
{
    cleanup();
}

void VertexBuffer::init(
    VkDeviceSize bufferSize,
    uint32_t     bufferCount
) {
    m_bufferSize  = bufferSize;
    m_bufferCount = bufferCount;

    // Initialize Vertex Buffers
    for (uint32_t i = 0; i < bufferCount; ++i)
    {
        m_buffers.emplace_back(std::make_unique<Buffer>(m_context));
        m_buffers.back()->init(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
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

void VertexBuffer::cleanup()
{
    for (auto &buffer : m_buffers)
        if (buffer) buffer->cleanup();

    for (auto &staging : m_stagingBuffers)
        if (staging) staging->cleanup();
    
    m_buffers.clear();
    m_stagingBuffers.clear();
}

void VertexBuffer::bind(
    VkCommandBuffer commandBuffer,
    uint32_t        currentFrame
) {
    VkBuffer     buffers[] = { m_buffers[currentFrame]->getHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(
        commandBuffer,
        0,
        1,
        buffers,
        offsets
    );
}

void VertexBuffer::update(
    void     *bufferData,
    uint32_t currentFrame
) {
    m_stagingBuffers[currentFrame]->update(bufferData);
    m_stagingBuffers[currentFrame]->copyTo(*m_buffers[currentFrame]);
}
