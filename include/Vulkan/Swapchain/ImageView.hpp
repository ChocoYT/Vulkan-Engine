#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanImage;

class VulkanImageView
{
    public:
        ~VulkanImageView();

        static std::unique_ptr<VulkanImageView> CreateFromImage(
            const VulkanDevice &device,
            const VulkanImage  &image,
            VkImageViewType    viewType,
            VkImageAspectFlags aspectMask
        );

        const VkImageView GetHandle() const { return m_handle; }

    private:
        VulkanImageView(
            const VulkanDevice &device,
            VkImageView handle
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanImageView(const VulkanImageView&) = delete;
        VulkanImageView& operator=(const VulkanImageView&) = delete;
        
        // Safe Move Semantics
        VulkanImageView(VulkanImageView &&other) noexcept;
        VulkanImageView& operator=(VulkanImageView &&other) noexcept;

        const VulkanDevice &m_device;

        VkImageView m_handle = VK_NULL_HANDLE;
};
