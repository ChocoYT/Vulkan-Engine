#pragma once

#define GLFW_NO_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

struct VulkanSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

class Window;

class VulkanSurface;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanRenderPass;

class VulkanImage;
class VulkanImageView;
class VulkanFramebuffer;

class VulkanSwapchain
{
    public:
        ~VulkanSwapchain();

        static std::unique_ptr<VulkanSwapchain> Create(
            const Window               &window,
            const VulkanSurface        &surface,
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            uint32_t requestImageCount
        );
        
        void CreateImages(
            const VulkanPhysicalDevice &physicalDevice,
            VulkanMemoryAllocator      &allocator
        );
        void CreateImageViews();
        void CreateFramebuffers(const VulkanRenderPass &renderPass);

        const VkSwapchainKHR GetHandle() const { return m_handle; }

        // Getters
        const std::vector<std::unique_ptr<VulkanImage>>&       GetImages()       const { return m_images;       }
        const std::vector<std::unique_ptr<VulkanImageView>>&   GetImageViews()   const { return m_imageViews;   }
        const std::vector<std::unique_ptr<VulkanFramebuffer>>& GetFramebuffers() const { return m_framebuffers; }

        const VkExtent2D GetExtent() const { return m_extent; }
        const VkFormat   GetFormat() const { return m_format; }

    private:
        VulkanSwapchain(
            const VulkanDevice &device,
            VkSwapchainKHR handle,
            VkExtent2D     extent,
            VkFormat       format
        );

        void Cleanup();
        
        static VulkanSwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        static VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int windowWidth, int windowHeight);

        // Remove Copying Semantics
        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
        
        // Safe Move Semantics
        VulkanSwapchain(VulkanSwapchain &&other) noexcept;
        VulkanSwapchain& operator=(VulkanSwapchain &&other) noexcept;

        const VulkanDevice &m_device;

        VkSwapchainKHR m_handle = VK_NULL_HANDLE;
        
        // Resources
        uint32_t                                        m_imageCount = 0;
        std::vector<std::unique_ptr<VulkanImage>>       m_images;
        std::vector<std::unique_ptr<VulkanImageView>>   m_imageViews;
        std::vector<std::unique_ptr<VulkanFramebuffer>> m_framebuffers;

        // State
        VkExtent2D m_extent{0, 0};
        VkFormat   m_format = VK_FORMAT_UNDEFINED;
};
