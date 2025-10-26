#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <set>
#include <vector>

class Context;

class Device
{
    public:
        Device(const Context &context);
        ~Device();

        void init();
        void cleanup();

        const VkDevice getHandle() const { return m_handle; }

        const VkQueue getGraphicsQueue() const { return m_graphicsQueue; }
        const VkQueue getPresentQueue()  const { return m_presentQueue;  }

    private:
        // Queues
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue  = VK_NULL_HANDLE;

        VkDevice m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
