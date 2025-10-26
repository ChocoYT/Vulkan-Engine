#include "Vulkan/Swapchain/Swapchain.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

#include "window.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Resources/Image.hpp"
#include "Vulkan/Swapchain/ImageView.hpp"
#include "Vulkan/Swapchain/Framebuffer.hpp"

#include "Vulkan/RenderPass/RenderPass.hpp"

Swapchain::Swapchain(
    const Window    &window,
    const Context   &context,
    MemoryAllocator &allocator
) : m_window(window), m_context(context), m_allocator(allocator) {}

Swapchain::~Swapchain()
{
    cleanup();
}

void Swapchain::init(uint32_t requestImageCount)
{
    GLFWwindow       *window          = m_window.getHandle();
    VkSurfaceKHR     vkSurface        = m_context.getSurface().getHandle();
    VkPhysicalDevice vkPhysicalDevice = m_context.getPhysicalDevice().getHandle();
    VkDevice         vkDevice         = m_context.getDevice().getHandle();

    uint32_t graphicsQueueFamily = m_context.getPhysicalDevice().getGraphicsQueueFamily();
    uint32_t presentQueueFamily  = m_context.getPhysicalDevice().getPresentQueueFamily();

    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    SwapChainSupportDetails swapChainSupport = Swapchain::querySwapchainSupport(vkPhysicalDevice, vkSurface);

    VkSurfaceFormatKHR surfaceFormat = Swapchain::chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR   presentMode   = Swapchain::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D         extent        = Swapchain::chooseSwapExtent(swapChainSupport.capabilities, width, height);

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        requestImageCount > swapChainSupport.capabilities.maxImageCount)
    {
        requestImageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Create Info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = vkSurface;
    createInfo.minImageCount    = requestImageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::vector<uint32_t> queueFamilyIndices;
    std::set<uint32_t> uniqueQueueFamilies = {
        graphicsQueueFamily,
        presentQueueFamily
    };

    if (graphicsQueueFamily != presentQueueFamily)
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

    result = vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSwapchainKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Swapchain.");
    }

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(vkDevice, m_handle, &imageCount, nullptr);

    std::vector<VkImage> vkImages(imageCount, VK_NULL_HANDLE);
    vkGetSwapchainImagesKHR(vkDevice, m_handle, &imageCount, vkImages.data());

    m_extent = extent;
    m_format = surfaceFormat.format;

    for (uint32_t i = 0; i < imageCount; ++i)
    {
        m_images.emplace_back(std::make_unique<Image>(m_context, m_allocator));
        m_images.back()->init(
            VkExtent3D{m_extent.width, m_extent.height, 1},
            m_format,
            1, 1,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vkImages[i]
        );
    }

    std::cout << "[INFO]\tSwapchain Created Successfully with " << imageCount << " Images.\n";
}

void Swapchain::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    // Destroy Framebuffers
    for (auto &framebuffer : m_framebuffers)
        framebuffer->cleanup();

    m_framebuffers.clear();

    // Destroy Image Views
    for (auto &imageView : m_imageViews)
        imageView->cleanup();
    
    m_imageViews.clear();

    for (auto &image : m_images)
        image->cleanup();

    m_images.clear();

    // Destroy Swapchain
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }

    m_images.clear();
}

void Swapchain::createImageViews()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    for (size_t i = 0; i < m_images.size(); ++i)
    {
        m_imageViews.emplace_back(std::make_unique<ImageView>(m_context));
        m_imageViews.back()->init(m_images[i]->getHandle(), m_images[i]->getFormat());
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";
}

void Swapchain::createFramebuffers(const RenderPass &renderPass)
{
    VkDevice     vkDevice     = m_context.getDevice().getHandle();
    VkRenderPass vkRenderPass = renderPass.getHandle();

    VkResult result = VK_SUCCESS;

    for (size_t i = 0; i < m_images.size(); ++i)
    {
        std::vector<VkImageView> attachments = {
            m_imageViews[i]->getHandle()
        };

        m_framebuffers.emplace_back(std::make_unique<Framebuffer>(m_context, renderPass));
        m_framebuffers.back()->init(attachments, m_extent);
    }

    std::cout << "[INFO]\tSwapchain Framebuffers Created Successfully.\n";
}

SwapChainSupportDetails Swapchain::querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

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

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
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

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto& mode : availablePresentModes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int windowWidth, int windowHeight)
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
