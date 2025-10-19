#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

class PhysicalDevice
{
    public:
        void init(
            VkInstance   instance,
            VkSurfaceKHR surface
        );

        const VkPhysicalDevice getHandle() const { return m_handle; }

        const uint32_t getGraphicsQueueFamily() const { return m_graphicsQueueFamily; }
        const uint32_t getPresentQueueFamily()  const { return m_presentQueueFamily;  }

    private:
        bool isDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device);
        void findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device);

        // Queue Families
        uint32_t m_graphicsQueueFamily = UINT32_MAX;
        uint32_t m_presentQueueFamily  = UINT32_MAX;

        VkPhysicalDevice m_handle = VK_NULL_HANDLE;
};
