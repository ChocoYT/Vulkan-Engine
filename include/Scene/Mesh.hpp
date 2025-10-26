#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

class Context;
class CommandPool;
class MemoryAllocator;

class VertexBuffer;
class IndexBuffer;
class Mesh
{
    public:
        Mesh(
            const Context     &context,
            const CommandPool &commandPool,
            MemoryAllocator   &allocator
        );
        ~Mesh();

        void init(
            const std::vector<Vertex>   &vertices,
            const std::vector<uint32_t> &indices,
            uint32_t bufferCount
        );
        void cleanup();

        void bind(
            VkCommandBuffer commandBuffer,
            uint32_t        currentFrame
        );
        void update(
            const std::vector<Vertex>   &vertices,
            const std::vector<uint32_t> &indices,
            uint32_t currentFrame
        );
        void draw(VkCommandBuffer commandBuffer);

    private:
        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::unique_ptr<IndexBuffer>  m_indexBuffer;

        uint32_t m_vertexCount = 0;
        uint32_t m_indexCount  = 0;

        const Context     &m_context;
        const CommandPool &m_commandPool;
        MemoryAllocator   &m_allocator;
};
