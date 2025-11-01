#include "Scene/Mesh.hpp"

#include "Vulkan/Buffers/Vertex.hpp"
#include "Vulkan/Buffers/Index.hpp"

VulkanMesh::VulkanMesh(
    std::unique_ptr<VulkanVertexBuffer> vertexBuffer,
    std::unique_ptr<VulkanIndexBuffer>  indexBuffer
) : m_vertexBuffer(std::move(vertexBuffer)),
    m_indexBuffer(std::move(indexBuffer))
{
    m_vertexCount = m_vertexBuffer->GetSize() / sizeof(Vertex);
    m_indexCount  = m_indexBuffer->GetSize() / sizeof(uint32_t);
}

VulkanMesh::~VulkanMesh() = default;

std::unique_ptr<VulkanMesh> VulkanMesh::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize vertexSize,
    VkDeviceSize indexSize,
    uint32_t     bufferCount
) {
    auto vertexBuffer = VulkanVertexBuffer::Create(
        physicalDevice,
        device,
        allocator,
        vertexSize,
        bufferCount
    );

    auto indexBuffer = VulkanIndexBuffer::Create(
        physicalDevice,
        device,
        allocator,
        indexSize,
        bufferCount
    );

    return std::unique_ptr<VulkanMesh>(
        new VulkanMesh(
            std::move(vertexBuffer),
            std::move(indexBuffer)
        )
    );
}

void VulkanMesh::Bind(
    VkCommandBuffer vkCommandBuffer,
    uint32_t        currentFrame
) {
    m_vertexBuffer->Bind(vkCommandBuffer, currentFrame);
    m_indexBuffer->Bind(vkCommandBuffer, currentFrame);
}

void VulkanMesh::UpdateBuffers(
    const VulkanCommandPool &commandPool,
    const std::vector<Vertex>   &vertices,
    const std::vector<uint32_t> &indices,
    uint32_t currentFrame
) {
    m_vertexBuffer->Update(commandPool, (void*)vertices.data(), currentFrame);
    m_indexBuffer->Update(commandPool, (void*)indices.data(), currentFrame);
}

void VulkanMesh::Draw(VkCommandBuffer vkCommandBuffer)
{
    vkCmdDrawIndexed(vkCommandBuffer, m_indexCount, 1, 0, 0, 0);
}

VulkanMesh::VulkanMesh(VulkanMesh&& other) noexcept : 
    m_vertexBuffer(std::move(other.m_vertexBuffer)),
    m_indexBuffer(std::move(other.m_indexBuffer)),
    m_vertexCount(other.m_vertexCount),
    m_indexCount(other.m_indexCount)
{
    other = VulkanMesh{};
}

VulkanMesh& VulkanMesh::operator=(VulkanMesh &&other) noexcept
{
    if (this != &other)
    {
        m_vertexBuffer = std::move(other.m_vertexBuffer);
        m_indexBuffer  = std::move(other.m_indexBuffer);
        m_vertexCount = other.m_vertexCount;
        m_indexCount  = other.m_indexCount;

        other = VulkanMesh{};
    }

    return *this;
}
