#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class Context;

class DescriptorPool
{
    public:
        DescriptorPool(const Context &context);
        ~DescriptorPool();

        void init(uint32_t frameCount);
        void cleanup();

        const VkDescriptorPool getHandle() const { return m_handle; }

    private:
        VkDescriptorPool m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
