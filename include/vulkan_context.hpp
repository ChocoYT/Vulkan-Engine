#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>

#include "settings.hpp"
#include "window.hpp"

#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

class Window;

class VulkanContext
{
    public:
        void init(const Window &window);
        void cleanup();

        VkInstance   getInstance() const { return instance; }
        VkSurfaceKHR getSurface()  const { return surface; }

        VkInstance   instance = nullptr;
        VkSurfaceKHR surface  = nullptr;

    private:
        void createInstance();
        void createSurface(GLFWwindow* windowHandle);
};

#endif
