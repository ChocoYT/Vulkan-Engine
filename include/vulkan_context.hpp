#pragma once

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

class Window;

class VulkanContext
{
    public:
        void init(const Window& window);
        void cleanup();

        VkDebugUtilsMessengerEXT getDebugMessenger() const { return debugMessenger; }

        // Core Vulkan Object Getters
        VkInstance       getInstance()       const { return instance;       }
        VkSurfaceKHR     getSurface()        const { return surface;        }
        VkSwapchainKHR   getSwapchain()      const { return swapchain;      }
        VkRenderPass     getRenderPass()     const { return renderPass;     }
        VkDevice         getDevice()         const { return device;         }
        VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
        
        // Swapchain Resource Getters
        std::vector<VkImage>       getSwapchainImages()       const { return swapchainImages;       }
        std::vector<VkImageView>   getSwapchainImageViews()   const { return swapchainImageViews;   }
        std::vector<VkFramebuffer> getSwapchainFramebuffers() const { return swapchainFramebuffers; }

        // Queue Getters
        VkQueue getGraphicsQueue() const { return graphicsQueue; }
        VkQueue getPresentQueue()  const { return presentQueue;  }

        // Swapchain State
        VkFormat   getSwapchainImageFormat() const { return swapchainFormat; }
        VkExtent2D getSwapchainExtent()      const { return swapchainExtent; }
        
        // Command Buffer
        VkCommandBuffer beginFrame(int currentFrame);

        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;

        // Core Vulkan Objects
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        VkInstance       instance        = VK_NULL_HANDLE;
        VkSurfaceKHR     surface         = VK_NULL_HANDLE;
        VkDevice         device          = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice  = VK_NULL_HANDLE;
        VkSwapchainKHR   swapchain       = VK_NULL_HANDLE;
        VkRenderPass     renderPass      = VK_NULL_HANDLE;

        // Swapchain Resources
        std::vector<VkImage>       swapchainImages;
        std::vector<VkImageView>   swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFramebuffers;

        // Queues
        VkQueue  graphicsQueue = VK_NULL_HANDLE;
        VkQueue  presentQueue  = VK_NULL_HANDLE;

        uint32_t graphicsQueueFamily = UINT32_MAX;
        uint32_t presentQueueFamily  = UINT32_MAX;

        // Swapchain State
        VkFormat   swapchainFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D swapchainExtent{0, 0};

    private:
        // Setup
        void createInstance();
        void createDebugCallback();
        void createSurface(GLFWwindow* windowHandle);

        void pickPhysicalDevice();
        void createDevice();

        void createSwapchain(GLFWwindow* windowHandle);
        void createRenderPass();
        void createImageViews();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();

        // Helpers
        bool isDeviceSuitable(VkPhysicalDevice device);
        void findQueueFamilies(VkPhysicalDevice device);
};
