#include "mesh.hpp"

void Mesh::init(VulkanContext& vulkanContext, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    context = &vulkanContext;

    vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertexCount;

    indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indexCount;

    // Vertex Buffer
    VulkanBuffer stagingVertexBuffer;
    stagingVertexBuffer.init(
        *context, vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    stagingVertexBuffer.copyData(vertices.data(), vertexBufferSize);

    vertexBuffer.init(
        *context, vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    stagingVertexBuffer.copyTo(vertexBuffer);
    stagingVertexBuffer.cleanup();

    // Index Buffer
    VulkanBuffer stagingIndexBuffer;
    stagingIndexBuffer.init(
        *context, indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    stagingIndexBuffer.copyData(indices.data(), indexBufferSize);

    indexBuffer.init(
        *context, indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    stagingIndexBuffer.copyTo(indexBuffer);
    stagingIndexBuffer.cleanup();
}

void Mesh::cleanup()
{
    vertexBuffer.cleanup();
    indexBuffer.cleanup();
}

void Mesh::bind(VkCommandBuffer commandBuffer)
{
    VkBuffer     vertexBuffers[] = {vertexBuffer.getBuffer()};
    VkDeviceSize offsets[]       = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::draw(VkCommandBuffer commandBuffer)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}
