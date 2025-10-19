#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class Buffer;

class StagingBuffer
{
    public:
        StagingBuffer(Context &context);
        ~StagingBuffer();

        void init(VkDeviceSize bufferSize);
        void cleanup();

        void update(void *bufferData);
        void copyTo(const Buffer &dst);

    private:
        std::unique_ptr<Buffer> m_buffer;

        VkDeviceSize m_bufferSize = 0;

        Context &m_context;
};
