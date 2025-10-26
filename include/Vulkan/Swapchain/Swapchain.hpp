#pragma once

#define GLFW_NO_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class Window;
class Context;
class MemoryAllocator;
class RenderPass;

class Image;
class ImageView;
class Framebuffer;

class Swapchain
{
    public:
        Swapchain(
            const Window    &window,
            const Context   &context,
            MemoryAllocator &allocator
        );
        ~Swapchain();

        void init(uint32_t requestImageCount);
        void cleanup();
        
        void createImageViews();
        void createFramebuffers(const RenderPass &renderPass);

        const VkSwapchainKHR getHandle() const { return m_handle; }

        // Getters
        const std::vector<std::unique_ptr<Image>>&       getImages()       const { return m_images;       }
        const std::vector<std::unique_ptr<ImageView>>&   getImageViews()   const { return m_imageViews;   }
        const std::vector<std::unique_ptr<Framebuffer>>& getFramebuffers() const { return m_framebuffers; }

        const VkFormat   getFormat() const { return m_format; }
        const VkExtent2D getExtent() const { return m_extent; }

    private:
        static SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        static VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int windowWidth, int windowHeight);

        VkSwapchainKHR m_handle = VK_NULL_HANDLE;
        
        // Resources
        std::vector<std::unique_ptr<Image>>       m_images;
        std::vector<std::unique_ptr<ImageView>>   m_imageViews;
        std::vector<std::unique_ptr<Framebuffer>> m_framebuffers;

        // State
        VkExtent2D m_extent{0, 0};
        VkFormat   m_format = VK_FORMAT_UNDEFINED;

        const Window    &m_window;
        const Context   &m_context;
        MemoryAllocator &m_allocator;
};
