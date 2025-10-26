#pragma once

#include <vulkan/vulkan.h>

#include <iostream>

class Context;

class Debug
{
    public:
        Debug(const Context &context);
        ~Debug();

        void init();
        void cleanup();

        const VkDebugUtilsMessengerEXT getHandle() const { return m_handle; }

    private:
        VkDebugUtilsMessengerEXT m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
