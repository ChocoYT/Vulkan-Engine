#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class Buffer;
class StagingBuffer;

class IndexBuffer
{
    public:
        IndexBuffer(Context &context);
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

        Context &m_context;
};
