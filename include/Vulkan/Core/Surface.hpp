#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>

class Window;
class Context;

class Surface
{
    public:
        Surface(
            const Window  &window,
            const Context &context
        );
        ~Surface();

        void init();
        void cleanup();

        const VkSurfaceKHR getHandle() const { return m_handle; }

    private:
        VkSurfaceKHR m_handle = VK_NULL_HANDLE;

        const Window  &m_window;
        const Context &m_context;
};
