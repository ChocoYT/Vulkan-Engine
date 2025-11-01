#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;

class VulkanDevice
{
    public:
        static std::unique_ptr<VulkanDevice> Create(
            const VulkanPhysicalDevice &physicalDevice
        );

        ~VulkanDevice();

        const VkDevice GetHandle() const { return m_handle; }

        const VkQueue  GetGraphicsQueue()       const { return m_graphicsQueue; }
        const VkQueue  GetPresentQueue()        const { return m_presentQueue; }
        const uint32_t GetGraphicsQueueFamily() const { return m_graphicsQueueFamily; }
        const uint32_t GetPresentQueueFamily()  const { return m_presentQueueFamily; }

    private:
        VulkanDevice() = default;
        VulkanDevice(
            VkDevice handle,
            VkQueue  graphicsQueue,
            VkQueue  presentQueue,
            uint32_t graphicsQueueFamily,
            uint32_t presentQueueFamily
        );

        // Remove Copying Semantics
        VulkanDevice(const VulkanDevice&) = delete;
        VulkanDevice& operator=(const VulkanDevice&) = delete;
        
        // Safe Move Semantics
        VulkanDevice(VulkanDevice &&other) noexcept;
        VulkanDevice& operator=(VulkanDevice &&other) noexcept;

        void Cleanup();

        VkDevice m_handle = VK_NULL_HANDLE;

        // Queues
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue  = VK_NULL_HANDLE;

        // Queue Families
        uint32_t m_graphicsQueueFamily = UINT32_MAX;
        uint32_t m_presentQueueFamily  = UINT32_MAX;
};
