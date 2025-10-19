#pragma once

#define GLFW_NO_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class Swapchain
{
    public:
        void init(
            VkSurfaceKHR     surface,
            VkPhysicalDevice physicalDevice,
            VkDevice         device,
            GLFWwindow       *window,
            uint32_t         graphicsQueueFamily,
            uint32_t         presentQueueFamily,
            uint32_t         frameCount
        );
        void cleanup(VkDevice device);
        
        void createImageViews(VkDevice device);
        void createFramebuffers(VkDevice device, VkRenderPass renderPass);

        const VkSwapchainKHR getHandle() const { return m_handle; }

        // Getters
        const std::vector<VkImage>&       getImages()       const { return m_images;       }
        const std::vector<VkImageView>&   getImageViews()   const { return m_imageViews;   }
        const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }

        const VkFormat   getFormat() const { return m_format; }
        const VkExtent2D getExtent() const { return m_extent; }

    private:
        static SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        static VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int windowWidth, int windowHeight);

        // Resources
        std::vector<VkImage>       m_images;
        std::vector<VkImageView>   m_imageViews;
        std::vector<VkFramebuffer> m_framebuffers;

        // State
        VkFormat   m_format = VK_FORMAT_UNDEFINED;
        VkExtent2D m_extent{0, 0};

        VkSwapchainKHR m_handle = VK_NULL_HANDLE;
};
