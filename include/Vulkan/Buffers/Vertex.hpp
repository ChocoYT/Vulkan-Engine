#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanCommandPool;

class VulkanBuffer;
class VulkanStagingBuffer;

class VulkanVertexBuffer
{
    public:
        ~VulkanVertexBuffer();

        static std::unique_ptr<VulkanVertexBuffer> Create(
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
        VulkanVertexBuffer() = default;
        VulkanVertexBuffer(
            std::vector<std::unique_ptr<VulkanBuffer>>        buffers,
            std::vector<std::unique_ptr<VulkanStagingBuffer>> stagingBuffers,
            VkDeviceSize size,
            uint32_t     count
        );

        // Remove Copying Semantics
        VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
        VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;
        
        // Safe Move Semantics
        VulkanVertexBuffer(VulkanVertexBuffer &&other) noexcept;
        VulkanVertexBuffer& operator=(VulkanVertexBuffer &&other) noexcept;

        void Cleanup();

        std::vector<std::unique_ptr<VulkanBuffer>>        m_buffers;
        std::vector<std::unique_ptr<VulkanStagingBuffer>> m_stagingBuffers;

        VkDeviceSize m_size  = 0;
        uint32_t     m_count = 0;
};
