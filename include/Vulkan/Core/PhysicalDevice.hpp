#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanInstance;
class VulkanSurface;

class VulkanPhysicalDevice
{
    public:
        static std::unique_ptr<VulkanPhysicalDevice> Create(
            const VulkanInstance &instance,
            const VulkanSurface  &surface
        );

        ~VulkanPhysicalDevice();

        const VkPhysicalDevice GetHandle() const { return m_handle; }

        const uint32_t GetGraphicsQueueFamily() const { return m_graphicsQueueFamily; }
        const uint32_t GetPresentQueueFamily()  const { return m_presentQueueFamily;  }

    private:
        VulkanPhysicalDevice() = default;
        VulkanPhysicalDevice(
            VkPhysicalDevice handle,
            uint32_t         graphicsQueueFamily,
            uint32_t         presentQueueFamily
        );

        // Remove Copying Semantics
        VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
        VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;
        
        // Safe Move Semantics
        VulkanPhysicalDevice(VulkanPhysicalDevice &&other) noexcept;
        VulkanPhysicalDevice& operator=(VulkanPhysicalDevice &&other) noexcept;

        static bool               IsDeviceSuitable(VkSurfaceKHR vkSurface, VkPhysicalDevice vkPhysicalDevice);
        static QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR vkSurface, VkPhysicalDevice vkPhysicalDevice);

        VkPhysicalDevice m_handle = VK_NULL_HANDLE;

        // Queue Families
        uint32_t m_graphicsQueueFamily = VK_QUEUE_FAMILY_IGNORED;
        uint32_t m_presentQueueFamily  = VK_QUEUE_FAMILY_IGNORED;
};
