#pragma once

#include <memory>

#include <vulkan/vulkan.h>

using AllocationHandle = void*;

class Context;
class MemoryAllocator;

class Image
{
public:
    Image(
        const Context   &context,
        MemoryAllocator &allocator
    );
    ~Image();

    void init(
        VkExtent3D extent,
        VkFormat   format,
        uint32_t mipLevels,
        uint32_t arrayLayers,
        VkImageTiling         tiling,
        VkImageUsageFlags     usage,
        VkMemoryPropertyFlags properties,
        VkImage               externalHandle = VK_NULL_HANDLE
    );
    void cleanup();

    void transitionLayout(
        VkCommandBuffer    commandBuffer,
        VkImageLayout      oldLayout,
        VkImageLayout      newLayout,
        VkImageAspectFlags aspectMask,
        uint32_t mipLevels   = 1,
        uint32_t arrayLayers = 1
    );

    const VkImage          getHandle()           const { return m_handle;           }
    const AllocationHandle getAllocationHandle() const { return m_allocationHandle; }

    VkFormat getFormat() const { return m_format; }
    
private:
    VkImage          m_handle           = VK_NULL_HANDLE;
    AllocationHandle m_allocationHandle = nullptr;

    VkExtent3D            m_extent{0, 0, 0};
    VkFormat              m_format      = VK_FORMAT_UNDEFINED;
    uint32_t              m_mipLevels   = 1;
    uint32_t              m_arrayLayers = 1;
    VkImageTiling         m_tiling      = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags     m_usage       = 0;
    VkMemoryPropertyFlags m_properties  = 0;

    bool m_isSwapchainImage = false;

    const Context   &m_context;
    MemoryAllocator &m_allocator;
};
