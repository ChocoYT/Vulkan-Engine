#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanCommandPool;
class VulkanMemoryAllocator;

class VulkanBuffer;
class VulkanStagingBuffer;

class VulkanIndexBuffer
{
    public:
        ~VulkanIndexBuffer();

        static std::unique_ptr<VulkanIndexBuffer> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            VulkanMemoryAllocator      &allocator,
            VkDeviceSize size,
            uint32_t     count
        );

        void Bind(
            VkCommandBuffer vkCommandBuffer,
            uint32_t        currentFrame
        );
        
        void Update(
            const VulkanCommandPool &commandPool,
            void     *data,
            uint32_t currentFrame
        );
        
        // Getters
        VkDeviceSize GetSize()  const { return m_size; }
        uint32_t     GetCount() const { return m_count; }

    private:
        VulkanIndexBuffer() = default;
        VulkanIndexBuffer(
            std::vector<std::unique_ptr<VulkanBuffer>>        buffers,
            std::vector<std::unique_ptr<VulkanStagingBuffer>> stagingBuffers,
            VkDeviceSize size,
            uint32_t     count
        );

        // Remove Copying Semantics
        VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
        VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;
        
        // Safe Move Semantics
        VulkanIndexBuffer(VulkanIndexBuffer &&other) noexcept;
        VulkanIndexBuffer& operator=(VulkanIndexBuffer &&other) noexcept;

        void Cleanup();

        std::vector<std::unique_ptr<VulkanBuffer>>        m_buffers;
        std::vector<std::unique_ptr<VulkanStagingBuffer>> m_stagingBuffers;

        VkDeviceSize m_size  = 0;
        uint32_t     m_count = 0;
};
