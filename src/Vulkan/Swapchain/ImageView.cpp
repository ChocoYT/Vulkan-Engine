#include "Vulkan/Swapchain/ImageView.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Resources/Image.hpp"

VulkanImageView::VulkanImageView(
    const VulkanDevice &device,
    VkImageView handle
) : m_device(device),
    m_handle(handle)
{}

VulkanImageView::~VulkanImageView()
{
    Cleanup();
}

std::unique_ptr<VulkanImageView> VulkanImageView::CreateFromImage(
    const VulkanDevice &device,
    const VulkanImage  &image,
    VkImageViewType    viewType,
    VkImageAspectFlags aspectMask
) {
    VkResult result = VK_SUCCESS;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image    = image.GetHandle();
    createInfo.viewType = viewType;
    createInfo.format   = image.GetFormat();

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    
    createInfo.subresourceRange.aspectMask = aspectMask;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = image.GetMipLevels();
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = image.GetArrayLayers();
    
    // Create Image View
    VkImageView handle = VK_NULL_HANDLE;
    result = vkCreateImageView(device.GetHandle(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateImageView' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Image Views.");
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";

    return std::unique_ptr<VulkanImageView>(
        new VulkanImageView(
            device,
            handle
        )
    );
}

void VulkanImageView::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanImageView::VulkanImageView(VulkanImageView &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanImageView& VulkanImageView::operator=(VulkanImageView &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}
