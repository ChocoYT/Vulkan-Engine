#pragma once

#include <memory>

#include <vulkan/vulkan.h>

using VulkanAllocationHandle = void*;

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanCommandPool;

class VulkanBuffer
{
    public:
        ~VulkanBuffer();

        static std::unique_ptr<VulkanBuffer> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            VulkanMemoryAllocator      &allocator,
            VkDeviceSize          size,
            VkBufferUsageFlags    usage,
            VkMemoryPropertyFlags properties
        );

        void Update(
            const void   *data,
            VkDeviceSize size
        );
        void CopyTo(
            const VulkanCommandPool &commandPool,
            const VulkanBuffer      &dst
        );

        const VkBuffer               GetHandle()           const { return m_handle; }
        const VulkanAllocationHandle GetAllocationHandle() const { return m_allocationHandle; }
        const VkDeviceSize           GetSize()             const { return m_size; }
        const VkBufferUsageFlags     GetUsage()            const { return m_usage; }
        const VkMemoryPropertyFlags  GetProperties()       const { return m_properties; }

    private:
        VulkanBuffer(
            const VulkanDevice     &device,
            VulkanMemoryAllocator  &allocator,
            VkBuffer               handle,
            VulkanAllocationHandle allocationHandle,
            VkDeviceSize           size,
            VkBufferUsageFlags     usage,
            VkMemoryPropertyFlags  properties
        );

        // Remove Copying Semantics
        VulkanBuffer(const VulkanBuffer&) = delete;
        VulkanBuffer& operator=(const VulkanBuffer&) = delete;
        
        // Safe Move Semantics
        VulkanBuffer(VulkanBuffer &&other) noexcept;
        VulkanBuffer& operator=(VulkanBuffer &&other) noexcept;

        void Cleanup();

        const VulkanDevice    &m_device;
        VulkanMemoryAllocator &m_allocator;

        VkBuffer               m_handle           = VK_NULL_HANDLE;
        VulkanAllocationHandle m_allocationHandle = nullptr;
        VkDeviceSize           m_size       = 0;
        VkBufferUsageFlags     m_usage      = 0;
        VkMemoryPropertyFlags  m_properties = 0;
};
