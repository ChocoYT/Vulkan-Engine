#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

class Context;

class Buffer
{
    public:
        Buffer(Context &context);
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

        // Buffer Getters
        const VkBuffer       getHandle() const { return m_handle; }
        const VkDeviceMemory getMemory() const { return m_memory; }
        const VkDeviceSize   getSize()   const { return m_size;   }

    private:
        VkBuffer       m_handle = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkDeviceSize   m_size   = 0;

        uint32_t findMemoryType(
            uint32_t              typeFilter,
            VkMemoryPropertyFlags properties
        );

        Context &m_context;
};
