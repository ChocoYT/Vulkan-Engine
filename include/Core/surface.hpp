#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>

class Surface
{
    public:
        void init(
            VkInstance  instance,
            GLFWwindow *window
        );
        void cleanup(VkInstance instance);

        const VkSurfaceKHR getHandle() const { return m_handle; }

    private:
        VkSurfaceKHR m_handle = VK_NULL_HANDLE;
};
