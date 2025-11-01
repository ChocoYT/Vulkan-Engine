#include "Vulkan/Swapchain/Swapchain.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

#include "Window/Window.hpp"

#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Resources/Image.hpp"
#include "Vulkan/Swapchain/ImageView.hpp"
#include "Vulkan/Swapchain/Framebuffer.hpp"

#include "Vulkan/RenderPass/RenderPass.hpp"

VulkanSwapchain::VulkanSwapchain(
    const VulkanDevice &device,
    VkSwapchainKHR handle,
    VkExtent2D     extent,
    VkFormat       format
) : m_device(device),
    m_handle(handle),
    m_extent(extent),
    m_format(format)
{}

VulkanSwapchain::~VulkanSwapchain()
{
    Cleanup();
}

std::unique_ptr<VulkanSwapchain> VulkanSwapchain::Create(
    const Window               &window,
    const VulkanSurface        &surface,
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    uint32_t requestImageCount
) {
    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(window.GetHandle(), &width, &height);

    VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapchain::QuerySwapchainSupport(physicalDevice.GetHandle(), surface.GetHandle());

    VkSurfaceFormatKHR surfaceFormat = VulkanSwapchain::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR   presentMode   = VulkanSwapchain::ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D         extent        = VulkanSwapchain::ChooseSwapExtent(swapChainSupport.capabilities, width, height);

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        requestImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        requestImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Create Info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = surface.GetHandle();
    createInfo.minImageCount    = requestImageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::vector<uint32_t> queueFamilyIndices;
    std::set<uint32_t> uniqueQueueFamilies = {
        device.GetGraphicsQueueFamily(),
        device.GetPresentQueueFamily()
    };

    if (device.GetGraphicsQueueFamily() != device.GetPresentQueueFamily())
    {
        queueFamilyIndices.assign(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());
        
        createInfo.imageSharingMode    = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; 
        createInfo.pQueueFamilyIndices   = nullptr;
    }

    createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    // Create Swapchain
    VkSwapchainKHR handle = VK_NULL_HANDLE;
    result = vkCreateSwapchainKHR(device.GetHandle(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSwapchainKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Swapchain.");
    }

    std::cout << "[INFO]\tSwapchain Created Successfully\n";

    return std::unique_ptr<VulkanSwapchain>(
        new VulkanSwapchain(
            device,
            handle,
            extent,
            surfaceFormat.format
        )
    );
}

void VulkanSwapchain::Cleanup()
{
    m_framebuffers.clear();
    m_imageViews.clear();
    m_images.clear();

    // Destroy Swapchain
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void VulkanSwapchain::CreateImages(
    const VulkanPhysicalDevice &physicalDevice,
    VulkanMemoryAllocator      &allocator
) {
    vkGetSwapchainImagesKHR(m_device.GetHandle(), m_handle, &m_imageCount, nullptr);

    std::vector<VkImage> vkImages(m_imageCount, VK_NULL_HANDLE);
    vkGetSwapchainImagesKHR(m_device.GetHandle(), m_handle, &m_imageCount, vkImages.data());

    m_images.clear();

    for (uint32_t i = 0; i < m_imageCount; ++i)
    {
        m_images.emplace_back(VulkanImage::Create(
            physicalDevice,
            m_device,
            allocator,
            {m_extent.width, m_extent.height, 1},
            m_format,
            1, 1,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vkImages[i]
        ));
    }
}

void VulkanSwapchain::CreateImageViews()
{
    m_imageViews.clear();

    for (size_t i = 0; i < m_imageCount; ++i)
    {
        m_imageViews.emplace_back(VulkanImageView::CreateFromImage(
            m_device,
            *m_images[i],
            VK_IMAGE_VIEW_TYPE_2D,
            VK_IMAGE_ASPECT_COLOR_BIT
        ));
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";
}

void VulkanSwapchain::CreateFramebuffers(const VulkanRenderPass &renderPass)
{
    m_framebuffers.clear();

    for (size_t i = 0; i < m_imageCount; ++i)
    {
        std::vector<VkImageView> attachments = {
            m_imageViews[i]->GetHandle()
        };

        m_framebuffers.emplace_back(VulkanFramebuffer::Create(
            m_device,
            renderPass,
            attachments,
            m_extent
        ));
    }

    std::cout << "[INFO]\tSwapchain Framebuffers Created Successfully.\n";
}

VulkanSwapchain::VulkanSwapchain(VulkanSwapchain &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle),
    m_extent(other.m_extent),
    m_format(other.m_format)
{
    other.m_handle    = VK_NULL_HANDLE;
    other.m_extent    = {0, 0};
    other.m_format    = VK_FORMAT_UNDEFINED;
}

VulkanSwapchain& VulkanSwapchain::operator=(VulkanSwapchain &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;
        m_extent = other.m_extent;
        m_format = other.m_format;

        other.m_handle = VK_NULL_HANDLE;
        other.m_extent = {0, 0};
        other.m_format = VK_FORMAT_UNDEFINED;
    }

    return *this;
}

VulkanSwapChainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VulkanSwapChainSupportDetails details;

    // Capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // Formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // Present Modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto& format : availableFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    // Fallback to First Available
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int windowWidth, int windowHeight)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = { static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight) };
        actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}
