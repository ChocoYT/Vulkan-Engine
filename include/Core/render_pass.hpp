#pragma once

#include <vulkan/vulkan.h>

#include <iostream>

class RenderPass
{
    public:
        void init(
            VkDevice       device,
            VkSwapchainKHR swapchain,
            VkFormat swapchainFormat
        );
        void cleanup(VkDevice device);

        const VkRenderPass getHandle() const { return m_handle; }

    private:
        VkRenderPass m_handle = VK_NULL_HANDLE;
};
