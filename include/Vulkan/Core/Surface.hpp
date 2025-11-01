#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class Window;
class VulkanInstance;

class VulkanSurface
{
    public:
        ~VulkanSurface();

        static std::unique_ptr<VulkanSurface> Create(
            const Window         &window,
            const VulkanInstance &instance
        );

        const VkSurfaceKHR GetHandle() const { return m_handle; }

    private:
        VulkanSurface(
            const VulkanInstance &instance,
            VkSurfaceKHR         handle
        );

        // Remove Copying Semantics
        VulkanSurface(const VulkanSurface&) = delete;
        VulkanSurface& operator=(const VulkanSurface&) = delete;
        
        // Safe Move Semantics
        VulkanSurface(VulkanSurface &&other) noexcept;
        VulkanSurface& operator=(VulkanSurface &&other) noexcept;

        void Cleanup();

        const VulkanInstance &m_instance;

        VkSurfaceKHR m_handle = VK_NULL_HANDLE;
};
