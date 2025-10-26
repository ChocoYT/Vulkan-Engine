#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class Context;
class Swapchain;
class RenderPass;
class Synchronization;

class ShaderModule;

class Pipeline
{
    public:
        Pipeline(
            const Context         &context,
            const Swapchain       &swapchain,
            const RenderPass      &renderPass,
            const Synchronization &synchronization
        );
        ~Pipeline();

        void init(
            std::vector<VkVertexInputBindingDescription>   &bindingDescs,
            std::vector<VkVertexInputAttributeDescription> &attrDescs,
            std::vector<VkDescriptorSetLayout>             &layoutDescs,
            uint32_t frameCount
        );
        void cleanup();

        void bind(VkCommandBuffer commandBuffer);

        uint32_t beginFrame(VkCommandBuffer commandBuffer, uint32_t currentFrame);
        void     endFrame(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t imageIndex);

        // Pipeline Getters
        VkPipeline       getPipeline() const { return m_handle; }
        VkPipelineLayout getLayout()   const { return m_layout; }

    private:
        // Pipeline
        VkPipeline       m_handle = VK_NULL_HANDLE;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        // Descriptions
        std::vector<VkVertexInputBindingDescription>   m_bindingDescs;
        std::vector<VkVertexInputAttributeDescription> m_attrDescs;
        std::vector<VkDescriptorSetLayout>             m_layoutDescs;

        // Shader Modules
        std::unique_ptr<ShaderModule> m_vertexShaderModule;
        std::unique_ptr<ShaderModule> m_pixelShaderModule;

        const Context         &m_context;
        const Swapchain       &m_swapchain;
        const RenderPass      &m_renderPass;
        const Synchronization &m_synchronization;
};
