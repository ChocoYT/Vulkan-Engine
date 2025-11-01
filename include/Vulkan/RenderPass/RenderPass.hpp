#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;

class VulkanRenderPass
{
    public:
        ~VulkanRenderPass();

        static std::unique_ptr<VulkanRenderPass> Create(
            const VulkanDevice    &device,
            const VulkanSwapchain &swapchain
        );

        const VkRenderPass GetHandle() const { return m_handle; }

    private:
        VulkanRenderPass(
            const VulkanDevice &device,
            VkRenderPass handle
        );

        // Remove Copying Semantics
        VulkanRenderPass(const VulkanRenderPass&) = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
        
        // Safe Move Semantics
        VulkanRenderPass(VulkanRenderPass &&other) noexcept;
        VulkanRenderPass& operator=(VulkanRenderPass &&other) noexcept;

        void Cleanup();

        const VulkanDevice &m_device;

        VkRenderPass m_handle = VK_NULL_HANDLE;
};
