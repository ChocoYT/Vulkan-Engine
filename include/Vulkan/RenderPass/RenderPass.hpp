#pragma once

#include <vulkan/vulkan.h>

#include <iostream>

class Context;
class Swapchain;

class RenderPass
{
    public:
        RenderPass(
            const Context   &context,
            const Swapchain &swapchain
        );
        ~RenderPass();

        void init();
        void cleanup();

        const VkRenderPass getHandle() const { return m_handle; }

    private:
        VkRenderPass m_handle = VK_NULL_HANDLE;

        const Context   &m_context;
        const Swapchain &m_swapchain;
};
