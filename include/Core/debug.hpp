#pragma once

#include <vulkan/vulkan.h>

#include <iostream>

class Debug
{
    public:
        void init(VkInstance instance);
        void cleanup(VkInstance instance);

        const VkDebugUtilsMessengerEXT getHandle() const { return m_handle; }

    private:
        VkDebugUtilsMessengerEXT m_handle = VK_NULL_HANDLE;
};
