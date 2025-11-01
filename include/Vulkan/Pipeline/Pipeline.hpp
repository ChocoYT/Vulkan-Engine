#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanSync;

class VulkanShaderModule;

class VulkanPipeline
{
    public:
        ~VulkanPipeline();

        static std::unique_ptr<VulkanPipeline> Create(
            const VulkanDevice     &device,
            const VulkanSwapchain  &swapchain,
            const VulkanRenderPass &renderPass,
            std::vector<VkVertexInputBindingDescription>   &bindingDescs,
            std::vector<VkVertexInputAttributeDescription> &attrDescs,
            std::vector<VkDescriptorSetLayout>             &layoutDescs,
            uint32_t frameCount
        );

        void Bind(
            VkCommandBuffer vkCommandBuffer,
            const std::vector<VkDescriptorSet> &vkDescriptorSets
        );

        uint32_t BeginFrame(
            const VulkanSwapchain  &swapchain,
            const VulkanRenderPass &renderPass,
            const VulkanSync       &sync,
            VkCommandBuffer vkCommandBuffer,
            uint32_t        currentFrame
        );
        void EndFrame(
            const VulkanSwapchain &swapchain,
            const VulkanSync      &sync,
            VkCommandBuffer vkCommandBuffer,
            uint32_t        currentFrame,
            uint32_t        imageIndex
        );

        // Pipeline Getters
        VkPipeline       GetPipeline() const { return m_handle; }
        VkPipelineLayout GetLayout()   const { return m_layout; }

    private:
        VulkanPipeline(
            const VulkanDevice &device,
            VkPipeline       handle,
            VkPipelineLayout layout,
            std::unique_ptr<VulkanShaderModule> vertexShaderModule,
            std::unique_ptr<VulkanShaderModule> pixelShaderModule
        );

        // Remove Copying Semantics
        VulkanPipeline(const VulkanPipeline&) = delete;
        VulkanPipeline& operator=(const VulkanPipeline&) = delete;
        
        // Safe Move Semantics
        VulkanPipeline(VulkanPipeline &&other) noexcept;
        VulkanPipeline& operator=(VulkanPipeline &&other) noexcept;

        void Cleanup();

        const VulkanDevice &m_device;

        VkPipeline       m_handle = VK_NULL_HANDLE;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        // Shader Modules
        std::unique_ptr<VulkanShaderModule> m_vertexShaderModule;
        std::unique_ptr<VulkanShaderModule> m_pixelShaderModule;
};
