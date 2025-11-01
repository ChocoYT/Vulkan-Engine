#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanDescriptorPool
{
    public:
        ~VulkanDescriptorPool();

        static std::unique_ptr<VulkanDescriptorPool> Create(
            const VulkanDevice &device,
            uint32_t frameCount
        );

        const VkDescriptorPool GetHandle() const { return m_handle; }

    private:
        VulkanDescriptorPool(
            const VulkanDevice &device,
            VkDescriptorPool handle
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
        VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;
        
        // Safe Move Semantics
        VulkanDescriptorPool(VulkanDescriptorPool &&other) noexcept;
        VulkanDescriptorPool& operator=(VulkanDescriptorPool &&other) noexcept;

        const VulkanDevice &m_device;

        VkDescriptorPool m_handle = VK_NULL_HANDLE;
};
