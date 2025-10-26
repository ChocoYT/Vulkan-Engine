#include "Vulkan/Swapchain/ImageView.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

ImageView::ImageView(const Context &context) : m_context(context) {}
ImageView::~ImageView()
{
    cleanup();
}

void ImageView::init(
    VkImage  vkImage,
    VkFormat vkFormat
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image    = vkImage;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format   = vkFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    
    result = vkCreateImageView(vkDevice, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateImageView' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Image Views.");
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";
}

void ImageView::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyImageView(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
