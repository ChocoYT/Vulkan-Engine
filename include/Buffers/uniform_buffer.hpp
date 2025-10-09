#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "camera.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_pipeline.hpp"

class UniformBuffer
{
    public:
        void init(VulkanContext &context, VkDeviceSize bufferSize);
        void cleanup(VulkanContext &context);

        void update(int currentFrame, void *bufferData);
        void bind(VulkanContext &vulkanContext, VulkanPipeline &vulkanPipeline, int currentFrame);

        // Getters
        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        std::vector<VulkanBuffer>    uniformBuffers;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDeviceSize bufferSize;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
};
