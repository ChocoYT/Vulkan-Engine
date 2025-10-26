#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class CommandPool;
class MemoryAllocator;

class Buffer;

class StagingBuffer
{
    public:
        StagingBuffer(
            const Context     &context,
            const CommandPool &commandPool,
            MemoryAllocator   &allocator
        );
        ~StagingBuffer();

        void init(VkDeviceSize bufferSize);
        void cleanup();

        void update(void *bufferData);
        void copyTo(const Buffer &dst);

    private:
        std::unique_ptr<Buffer> m_buffer;

        VkDeviceSize m_bufferSize = 0;

        const Context     &m_context;
        const CommandPool &m_commandPool;
        MemoryAllocator   &m_allocator;
};
