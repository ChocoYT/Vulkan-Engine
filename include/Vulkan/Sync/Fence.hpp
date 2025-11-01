#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanFence
{
    public:
        ~VulkanFence();

        static std::unique_ptr<VulkanFence> Create(
            const VulkanDevice &device,
            VkFenceCreateFlags flags
        );

        void Wait();

        const VkFence GetHandle() const { return m_handle; }

    private:
        VulkanFence(
            const VulkanDevice &device,
            VkFence            handle,
            VkFenceCreateFlags flags
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanFence(const VulkanFence&) = delete;
        VulkanFence& operator=(const VulkanFence&) = delete;
        
        // Safe Move Semantics
        VulkanFence(VulkanFence &&other) noexcept;
        VulkanFence& operator=(VulkanFence &&other) noexcept;

        const VulkanDevice &m_device;

        VkFence m_handle = VK_NULL_HANDLE;

        VkFenceCreateFlags m_flags = 0;
};
