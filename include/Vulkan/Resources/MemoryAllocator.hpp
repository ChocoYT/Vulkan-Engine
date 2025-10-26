#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

using AllocationHandle = void*;

class Context;

class MemoryAllocator
{
    public:
        MemoryAllocator(const Context &context);
        ~MemoryAllocator();

        void free(AllocationHandle handle);

        AllocationHandle allocate(
            VkDeviceSize          size,
            uint32_t              memoryTypeBits,
            VkMemoryPropertyFlags properties
        );

        void* map(AllocationHandle handle);
        void  unmap(AllocationHandle handle);

        void bindBuffer(
            VkBuffer         vkBuffer, 
            AllocationHandle handle
        );
        void bindImage(
            VkImage          vkImage,
            AllocationHandle handle
        );

    private:
        // Disallow Copying and Assignment
        MemoryAllocator(const MemoryAllocator&)            = delete;
        MemoryAllocator& operator=(const MemoryAllocator&) = delete;

        uint32_t findMemoryType(
            uint32_t              typeFilter, 
            VkMemoryPropertyFlags properties
        ) const;

        const Context &m_context;
};
