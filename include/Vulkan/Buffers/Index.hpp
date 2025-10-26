#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class CommandPool;
class MemoryAllocator;

class Buffer;
class StagingBuffer;

class IndexBuffer
{
    public:
        IndexBuffer(
            const Context     &context,
            const CommandPool &commandPool,
            MemoryAllocator   &allocator
        );
        ~IndexBuffer();

        void init(
            VkDeviceSize bufferSize,
            uint32_t     bufferCount
        );
        void cleanup();

        void bind(
            VkCommandBuffer commandBuffer,
            uint32_t        currentFrame
        );
        void update(
            void     *bufferData,
            uint32_t currentFrame
        );

    private:
        std::vector<std::unique_ptr<Buffer>>        m_buffers;
        std::vector<std::unique_ptr<StagingBuffer>> m_stagingBuffers;

        VkDeviceSize m_bufferSize  = 0;
        uint32_t     m_bufferCount = 0;

        const Context     &m_context;
        const CommandPool &m_commandPool;
        MemoryAllocator   &m_allocator;
};
