#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanSemaphore
{
    public:
        ~VulkanSemaphore();

        static std::unique_ptr<VulkanSemaphore> Create(
            const VulkanDevice &device,
            VkSemaphoreType type
        );

        const VkSemaphore GetHandle() const { return m_handle; }

    private:
        VulkanSemaphore(
            const VulkanDevice &device,
            VkSemaphore     handle,
            VkSemaphoreType type
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanSemaphore(const VulkanSemaphore&) = delete;
        VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;
        
        // Safe Move Semantics
        VulkanSemaphore(VulkanSemaphore &&other) noexcept;
        VulkanSemaphore& operator=(VulkanSemaphore &&other) noexcept;

        const VulkanDevice &m_device;
        
        VkSemaphore m_handle = VK_NULL_HANDLE;

        VkSemaphoreType m_type = VK_SEMAPHORE_TYPE_BINARY;
};
