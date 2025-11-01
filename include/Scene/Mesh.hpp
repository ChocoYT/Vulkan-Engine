#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanCommandPool;
class VulkanMemoryAllocator;

class VulkanVertexBuffer;
class VulkanIndexBuffer;
class VulkanMesh
{
    public:
        ~VulkanMesh();

        static std::unique_ptr<VulkanMesh> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            VulkanMemoryAllocator      &allocator,
            VkDeviceSize vertexSize,
            VkDeviceSize indexSize,
            uint32_t     bufferCount
        );

        void Bind(
            VkCommandBuffer vkCommandBuffer,
            uint32_t        currentFrame
        );

        void UpdateBuffers(
            const VulkanCommandPool &commandPool,
            const std::vector<Vertex>   &vertices,
            const std::vector<uint32_t> &indices,
            uint32_t currentFrame
        );

        void Draw(VkCommandBuffer vkCommandBuffer);

    private:
        VulkanMesh() = default;
        VulkanMesh(
            std::unique_ptr<VulkanVertexBuffer> vertexBuffer,
            std::unique_ptr<VulkanIndexBuffer>  indexBuffer
        );

        // Remove Copying Semantics
        VulkanMesh(const VulkanMesh&) = delete;
        VulkanMesh& operator=(const VulkanMesh&) = delete;
        
        // Safe Move Semantics
        VulkanMesh(VulkanMesh &&other) noexcept;
        VulkanMesh& operator=(VulkanMesh &&other) noexcept;
        
        std::unique_ptr<VulkanVertexBuffer> m_vertexBuffer;
        std::unique_ptr<VulkanIndexBuffer>  m_indexBuffer;

        uint32_t m_vertexCount = 0;
        uint32_t m_indexCount  = 0;
};
