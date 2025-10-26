#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class Context;

class CommandPool
{
    public:
        CommandPool(const Context &context);
        ~CommandPool();

        void init();
        void cleanup();

        void createCommandBuffers(uint32_t frameCount);
        VkCommandBuffer beginFrame(uint32_t currentFrame);

        VkCommandPool getHandle() const { return m_handle; }

        const std::vector<VkCommandBuffer>& getCommandBuffers() const { return m_commandBuffers; }

    private:
        std::vector<VkCommandBuffer> m_commandBuffers;

        VkCommandPool m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
