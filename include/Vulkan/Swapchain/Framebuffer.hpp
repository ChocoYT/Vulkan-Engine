#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanRenderPass;

class VulkanFramebuffer
{
    public:
        ~VulkanFramebuffer();

        static std::unique_ptr<VulkanFramebuffer> Create(
            const VulkanDevice             &device,
            const VulkanRenderPass         &renderPass,
            const std::vector<VkImageView> &attachments,
            VkExtent2D extent
        );

        const VkFramebuffer GetHandle() const { return m_handle; }

    private:
        VulkanFramebuffer(
            const VulkanDevice &device,
            VkFramebuffer handle
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanFramebuffer(const VulkanFramebuffer&) = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;
        
        // Safe Move Semantics
        VulkanFramebuffer(VulkanFramebuffer &&other) noexcept;
        VulkanFramebuffer& operator=(VulkanFramebuffer &&other) noexcept;

        const VulkanDevice &m_device;

        VkFramebuffer m_handle = VK_NULL_HANDLE;
};
