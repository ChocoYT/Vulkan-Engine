#pragma once

#include <vulkan/vulkan.h>

class Context;

class Semaphore
{
    public:
        Semaphore(const Context &context);
        ~Semaphore();

        void init(VkSemaphoreType type);
        void cleanup();

        const VkSemaphore getHandle() const { return m_handle; }

    private:
        VkSemaphore m_handle;

        const Context &m_context;
};
