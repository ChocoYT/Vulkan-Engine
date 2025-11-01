#pragma once

#include <memory>

#include <vulkan/vulkan.h>

class VulkanInstance;

class VulkanDebug
{
    public:
        ~VulkanDebug();

        static std::unique_ptr<VulkanDebug> Create(
            const VulkanInstance &instance
        );

        const VkDebugUtilsMessengerEXT GetHandle() const { return m_handle; }

    private:
        VulkanDebug(
            const VulkanInstance     &instance,
            VkDebugUtilsMessengerEXT handle
        );

        // Remove Copying Semantics
        VulkanDebug(const VulkanDebug&) = delete;
        VulkanDebug& operator=(const VulkanDebug&) = delete;
        
        // Safe Move Semantics
        VulkanDebug(VulkanDebug &&other) noexcept;
        VulkanDebug& operator=(VulkanDebug &&other) noexcept;

        void Cleanup();

        const VulkanInstance &m_instance;

        VkDebugUtilsMessengerEXT m_handle = VK_NULL_HANDLE;
};
