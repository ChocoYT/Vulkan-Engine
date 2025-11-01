#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanDescriptorPool;
class VulkanPipeline;

class VulkanBuffer;

class VulkanUniformBuffer
{
    public:
        ~VulkanUniformBuffer();

        static std::unique_ptr<VulkanUniformBuffer> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            const VulkanDescriptorPool &descriptorPool,
            VulkanMemoryAllocator      &allocator,
            VkDeviceSize size,
            uint32_t     count
        );

        void Bind(
            VulkanPipeline  &pipeline,
            VkCommandBuffer vkCommandBuffer,
            uint32_t        currentFrame
        );
        
        void Update(
            void     *data,
            uint32_t currentFrame
        );

        // Getters
        VkDescriptorSet       GetDescriptorSets(uint32_t currentFrame) const { return m_descriptorSets[currentFrame]; }
        VkDescriptorSetLayout GetDescriptorSetLayout()                 const { return m_descriptorSetLayout; }

        VkDeviceSize GetSize()  const { return m_size; }
        uint32_t     GetCount() const { return m_count; }

    private:
        VulkanUniformBuffer(
            const VulkanDevice &device,
            std::vector<std::unique_ptr<VulkanBuffer>> buffers,
            std::vector<VkDescriptorSet> descriptorSets,
            VkDescriptorSetLayout        descriptorSetLayout,
            VkDeviceSize size,
            uint32_t     count
        );

        // Remove Copying Semantics
        VulkanUniformBuffer(const VulkanUniformBuffer&) = delete;
        VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;
        
        // Safe Move Semantics
        VulkanUniformBuffer(VulkanUniformBuffer &&other) noexcept;
        VulkanUniformBuffer& operator=(VulkanUniformBuffer &&other) noexcept;

        void Cleanup();

        const VulkanDevice &m_device;

        std::vector<std::unique_ptr<VulkanBuffer>> m_buffers;

        std::vector<VkDescriptorSet> m_descriptorSets;
        VkDescriptorSetLayout        m_descriptorSetLayout = VK_NULL_HANDLE;

        VkDeviceSize m_size  = 0;
        uint32_t     m_count = 0;
};
