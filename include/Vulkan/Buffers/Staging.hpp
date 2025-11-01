#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanCommandPool;

class VulkanBuffer;

class VulkanStagingBuffer
{
    public:
        ~VulkanStagingBuffer();

        static std::unique_ptr<VulkanStagingBuffer> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            VulkanMemoryAllocator      &allocator,
            VkDeviceSize size
        );

        void Update(void *data);

        void CopyTo(
            const VulkanCommandPool &commandPool,
            const VulkanBuffer      &dst
        );

        // Getters
        VkDeviceSize GetSize() const { return m_size; }

    private:
        VulkanStagingBuffer() = default;
        VulkanStagingBuffer(
            std::unique_ptr<VulkanBuffer> buffer,
            VkDeviceSize size
        );

        // Remove Copying Semantics
        VulkanStagingBuffer(const VulkanStagingBuffer&) = delete;
        VulkanStagingBuffer& operator=(const VulkanStagingBuffer&) = delete;
        
        // Safe Move Semantics
        VulkanStagingBuffer(VulkanStagingBuffer &&other) noexcept;
        VulkanStagingBuffer& operator=(VulkanStagingBuffer &&other) noexcept;

        std::unique_ptr<VulkanBuffer> m_buffer;

        VkDeviceSize m_size = 0;
};
