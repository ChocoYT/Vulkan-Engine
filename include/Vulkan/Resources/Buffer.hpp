#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <vulkan/vulkan.h>

using AllocationHandle = void*;

class Context;
class CommandPool;
class MemoryAllocator;

class Buffer
{
    public:
        Buffer(
            const Context     &context,
            const CommandPool &commandPool,
            MemoryAllocator   &allocator
        );
        ~Buffer();

        void init(
            VkDeviceSize          bufferSize,
            VkBufferUsageFlags    usage,
            VkMemoryPropertyFlags properties
        );
        void cleanup();

        void update(
            const void   *srcData,
            VkDeviceSize dataSize
        );
        void copyTo(const Buffer &dst);

        const VkBuffer         getHandle()           const { return m_handle;           }
        const AllocationHandle getAllocationHandle() const { return m_allocationHandle; }

        const VkDeviceSize getSize() const { return m_size; }

    private:
        VkBuffer         m_handle           = VK_NULL_HANDLE;
        AllocationHandle m_allocationHandle = nullptr;

        VkDeviceSize          m_size       = 0;
        VkBufferUsageFlags    m_usage      = 0;
        VkMemoryPropertyFlags m_properties = 0;

        const Context     &m_context;
        const CommandPool &m_commandPool;
        MemoryAllocator   &m_allocator;
};
