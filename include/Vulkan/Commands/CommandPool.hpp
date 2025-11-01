#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanCommandPool;

class VulkanCommandPool
{
    public:
        ~VulkanCommandPool();

        static std::unique_ptr<VulkanCommandPool> Create(
            const VulkanDevice &device
        );

        void CreateCommandBuffers(uint32_t frameCount);
        VkCommandBuffer BeginFrame(uint32_t currentFrame);

        VkCommandPool GetHandle() const { return m_handle; }

        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return m_commandBuffers; }

    private:
        VulkanCommandPool(
            const VulkanDevice &device,
            VkCommandPool handle
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanCommandPool(const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
        
        // Safe Move Semantics
        VulkanCommandPool(VulkanCommandPool &&other) noexcept;
        VulkanCommandPool& operator=(VulkanCommandPool &&other) noexcept;

        const VulkanDevice &m_device;

        VkCommandPool m_handle = VK_NULL_HANDLE;

        std::vector<VkCommandBuffer> m_commandBuffers;
};
