#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class Context;
class Buffer;

class UniformBuffer
{
    public:
        UniformBuffer(Context &context);
        ~UniformBuffer();

        void init(
            VkDeviceSize bufferSize,
            uint32_t     bufferCount
        );
        void cleanup();

        void bind(
            VkCommandBuffer  commandBuffer,
            VkPipelineLayout pipelineLayout,
            uint32_t         currentFrame
        );
        void update(
            void     *bufferData,
            uint32_t currentFrame
        );

        // Getters
        VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

    private:
        std::vector<std::unique_ptr<Buffer>> m_buffers;

        std::vector<VkDescriptorSet> m_descriptorSets;
        VkDescriptorSetLayout        m_descriptorSetLayout = VK_NULL_HANDLE;

        VkDeviceSize m_bufferSize  = 0;
        uint32_t     m_bufferCount = 0;

        Context &m_context;
};
