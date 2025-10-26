#pragma once

#include <vulkan/vulkan.h>

class Context;

class Fence
{
    public:
        Fence(const Context &context);
        ~Fence();

        void init(VkFenceCreateFlags flags);
        void cleanup();

        void wait();

        const VkFence getHandle() const { return m_handle; }

    private:
        VkFence m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
