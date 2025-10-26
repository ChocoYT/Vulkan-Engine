#include "Scene/Mesh.hpp"

#include "Vulkan/Buffers/Vertex.hpp"
#include "Vulkan/Buffers/Index.hpp"

Mesh::Mesh(
    const Context     &context,
    const CommandPool &commandPool,
    MemoryAllocator   &allocator
) : m_context(context), m_commandPool(commandPool), m_allocator(allocator) {}

Mesh::~Mesh()
{
    cleanup();
}

void Mesh::init(
    const std::vector<Vertex>   &vertices,
    const std::vector<uint32_t> &indices,
    uint32_t bufferCount
) {
    m_vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * m_vertexCount;

    m_indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * m_indexCount;

    m_vertexBuffer = std::make_unique<VertexBuffer>(m_context, m_commandPool, m_allocator);
    m_vertexBuffer->init(vertexBufferSize, bufferCount);
    m_indexBuffer = std::make_unique<IndexBuffer>(m_context, m_commandPool, m_allocator);
    m_indexBuffer->init(indexBufferSize, bufferCount);

    for (uint32_t currentFrame = 0; currentFrame < bufferCount; ++currentFrame)
        update(vertices, indices, currentFrame);
}

void Mesh::cleanup()
{
    m_vertexBuffer->cleanup();
    m_indexBuffer->cleanup();
}

void Mesh::bind(
    VkCommandBuffer commandBuffer,
    uint32_t        currentFrame
) {
    m_vertexBuffer->bind(commandBuffer, currentFrame);
    m_indexBuffer->bind(commandBuffer, currentFrame);
}

void Mesh::update(
    const std::vector<Vertex>   &vertices,
    const std::vector<uint32_t> &indices,
    uint32_t currentFrame
) {
    m_vertexBuffer->update((void*)vertices.data(), currentFrame);
    m_indexBuffer->update((void*)indices.data(), currentFrame);
}

void Mesh::draw(VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
}
