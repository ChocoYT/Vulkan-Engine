#pragma once

#include <string>
#include <memory>

#include <vulkan/vulkan.h>

class VulkanInstance
{
    public:
        ~VulkanInstance();

        static std::unique_ptr<VulkanInstance> Create(
            const std::string &appName,
            uint32_t          appVersion,
            const std::string &engineName,
            uint32_t          engineVersion
        );

        const VkInstance GetHandle() const { return m_handle; }

    private:
        VulkanInstance() = default;
        VulkanInstance(
            VkInstance handle
        );

        // Remove Copying Semantics
        VulkanInstance(const VulkanInstance&) = delete;
        VulkanInstance& operator=(const VulkanInstance&) = delete;
        
        // Safe Move Semantics
        VulkanInstance(VulkanInstance &&other) noexcept;
        VulkanInstance& operator=(VulkanInstance &&other) noexcept;

        void Cleanup();

        VkInstance m_handle = VK_NULL_HANDLE;
};
