#include "mesh.hpp"

void Mesh::init(VulkanContext& vulkanContext, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    context = &vulkanContext;

    vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertexCount;

    vertexBuffer.initVertexBuffer(vulkanContext, (void*)vertices.data(), vertexBufferSize);

    indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indexCount;

    indexBuffer.initIndexBuffer(vulkanContext, (void*)indices.data(), indexBufferSize);
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
