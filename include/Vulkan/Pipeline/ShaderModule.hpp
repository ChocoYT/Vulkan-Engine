#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanShaderModule
{
    public:
        ~VulkanShaderModule();

        static std::unique_ptr<VulkanShaderModule> Create(
            const VulkanDevice &device,
            const std::string &filePath
        );

        const VkShaderModule GetHandle() const { return m_handle; }

    private:
        VulkanShaderModule(
            const VulkanDevice &device,
            VkShaderModule m_handle
        );

        // Remove Copying Semantics
        VulkanShaderModule(const VulkanShaderModule&) = delete;
        VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
        
        // Safe Move Semantics
        VulkanShaderModule(VulkanShaderModule &&other) noexcept;
        VulkanShaderModule& operator=(VulkanShaderModule &&other) noexcept;

        void Cleanup();

        const VulkanDevice &m_device;

        VkShaderModule m_handle = VK_NULL_HANDLE;
};
