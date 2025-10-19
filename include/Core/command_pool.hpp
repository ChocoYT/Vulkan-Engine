#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class CommandPool
{
    public:
        void init(
            VkDevice device,
            uint32_t graphicsQueueFamily
        );
        void cleanup(VkDevice device);

        void createCommandBuffers(
            VkDevice device,
            uint32_t frameCount
        );
        VkCommandBuffer beginFrame(uint32_t currentFrame);

        VkCommandPool getHandle() const { return m_handle; }

        const std::vector<VkCommandBuffer>& getCommandBuffers() const { return m_commandBuffers; }

    private:
        std::vector<VkCommandBuffer> m_commandBuffers;

        VkCommandPool m_handle = VK_NULL_HANDLE;
};
