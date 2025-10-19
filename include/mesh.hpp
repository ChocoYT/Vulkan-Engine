#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "Buffers/vertex.hpp"
#include "Buffers/index.hpp"
#include "Buffers/uniform.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

class Context;
class VertexBuffer;
class IndexBuffer;
class Mesh
{
    public:
        Mesh(Context &context);
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

        Context &m_context;
};
