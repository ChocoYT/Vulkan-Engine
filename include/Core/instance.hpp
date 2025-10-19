#pragma once

#define GLFW_NO_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class Instance
{
    public:
        void init();
        void cleanup();

        const VkInstance getHandle() const { return m_handle; }

    private:
        VkInstance m_handle = VK_NULL_HANDLE;
};
