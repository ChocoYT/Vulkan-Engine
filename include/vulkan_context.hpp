#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "settings.hpp"
#include "swapchain_utils.hpp"
#include "window.hpp"

#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

class Window;

class VulkanContext
{
    public:
        void init(const Window& window);
        void cleanup();

        VkInstance       getInstance()       const { return instance;       }
        VkSurfaceKHR     getSurface()        const { return surface;        }
        VkSwapchainKHR   getSwapchain()      const { return swapchain;      }
        VkDevice         getDevice()         const { return device;         }
        VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }

    private:
        // Vulkan Handles
        VkInstance       instance        = VK_NULL_HANDLE;
        VkSurfaceKHR     surface         = VK_NULL_HANDLE;
        VkDevice         device          = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice  = VK_NULL_HANDLE;
        VkSwapchainKHR   swapchain       = VK_NULL_HANDLE;

        // Swapchain State
        VkFormat   swapchainFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D swapchainExtent{0, 0};

        std::vector<VkImage>     swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        // Queues
        VkQueue  graphicsQueue = VK_NULL_HANDLE;
        VkQueue  presentQueue  = VK_NULL_HANDLE;
        uint32_t graphicsQueueFamily = UINT32_MAX;
        uint32_t presentQueueFamily  = UINT32_MAX;

        // Setup
        void createInstance();
        void createSurface(GLFWwindow* windowHandle);
        void pickPhysicalDevice();
        void createDevice();
        void createSwapchain(GLFWwindow* windowHandle);

        // Helpers
        bool isDeviceSuitable(VkPhysicalDevice device);
        void findQueueFamilies(VkPhysicalDevice device);
};

#endif
