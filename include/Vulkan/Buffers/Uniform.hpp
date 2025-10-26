#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class CommandPool;
class DescriptorPool;
class MemoryAllocator;

class Buffer;

class UniformBuffer
{
    public:
        UniformBuffer(
            const Context        &context,
            const CommandPool    &commandPool,
            const DescriptorPool &descriptorPool,
            MemoryAllocator      &allocator
        );
        ~UniformBuffer();

        void init(
            VkDeviceSize bufferSize,
            uint32_t     bufferCount
        );
        void cleanup();

        void bind(
            VkCommandBuffer  commandBuffer,
            VkPipelineLayout pipelineLayout,
            uint32_t         currentFrame
        );
        void update(
            void     *bufferData,
            uint32_t currentFrame
        );

        // Getters
        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        std::vector<std::unique_ptr<Buffer>> m_buffers;

        std::vector<VkDescriptorSet> m_descriptorSets;
        VkDescriptorSetLayout        m_descriptorSetLayout = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize  = 0;
        uint32_t     m_bufferCount = 0;

        const Context        &m_context;
        const CommandPool    &m_commandPool;
        const DescriptorPool &m_descriptorPool;
        MemoryAllocator      &m_allocator;
};
