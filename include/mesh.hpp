#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <vector>

#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

class Mesh
{
    public:
        void init(VulkanContext &vulkanContext, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        void cleanup();

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;

        uint32_t     vertexCount = 0;
        uint32_t     indexCount  = 0;

        VulkanContext *context;
};
