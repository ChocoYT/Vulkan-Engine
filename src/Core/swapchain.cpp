#include "Core/swapchain.hpp"

void Swapchain::init(
    VkSurfaceKHR     surface,
    VkPhysicalDevice physicalDevice,
    VkDevice         device,
    GLFWwindow       *window,
    uint32_t         graphicsQueueFamily,
    uint32_t         presentQueueFamily,
    uint32_t         requestImageCount
) {
    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    SwapChainSupportDetails swapChainSupport = Swapchain::querySwapchainSupport(physicalDevice, surface);

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
    createInfo.surface          = surface;
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

    result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSwapchainKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Swapchain.");
    }

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, m_handle, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_handle, &imageCount, m_images.data());

    m_format = surfaceFormat.format;
    m_extent = extent;

    std::cout << "[INFO]\tSwapchain Created Successfully with " << imageCount << " Images.\n";
}

void Swapchain::cleanup(VkDevice device)
{
    // Destroy Frame Buffers
    for (auto framebuffer : m_framebuffers)
        if (framebuffer != VK_NULL_HANDLE)
            vkDestroyFramebuffer(device, framebuffer, nullptr);
    m_framebuffers.clear();

    // Destroy Image Views
    for (auto imageView : m_imageViews)
        if (imageView != VK_NULL_HANDLE)
            vkDestroyImageView(device, imageView, nullptr);
    m_imageViews.clear();

    // Destroy Swapchain
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }

    m_images.clear();
}

void Swapchain::createImageViews(VkDevice device)
{
    VkResult result = VK_SUCCESS;

    m_imageViews.resize(m_images.size(), VK_NULL_HANDLE);

    for (size_t i = 0; i < m_imageViews.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_format;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        result = vkCreateImageView(device, &createInfo, nullptr, &m_imageViews[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "[ERROR]\t'vkCreateImageView' Failed with Error Code " << result << "\n";

            throw std::runtime_error("Failed to Create Image Views.");
        }
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";
}

void Swapchain::createFramebuffers(VkDevice device, VkRenderPass renderPass)
{
    VkResult result = VK_SUCCESS;

    m_framebuffers.resize(m_imageViews.size(), VK_NULL_HANDLE);

    for (size_t i = 0; i < m_framebuffers.size(); ++i)
    {
        std::vector<VkImageView> attachments = {
            m_imageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = m_extent.width;
        framebufferInfo.height          = m_extent.height;
        framebufferInfo.layers          = 1;

        result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "[ERROR]\t'vkCreateFramebuffer' Failed with Error Code " << result << "\n";

            throw std::runtime_error("Failed to Create Framebuffers.");
        }
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
