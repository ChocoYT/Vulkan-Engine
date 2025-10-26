#include "Vulkan/Resources/Image.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Resources/MemoryAllocator.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>

Image::Image(
    const Context   &context,
    MemoryAllocator &allocator
) : m_context(context), m_allocator(allocator) {}

Image::~Image()
{
    cleanup();
}

void Image::init(
    VkExtent3D extent,
    VkFormat   format,
    uint32_t   mipLevels,
    uint32_t   arrayLayers,
    VkImageTiling         tiling,
    VkImageUsageFlags     usage,
    VkMemoryPropertyFlags properties,
    VkImage               externalHandle
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    m_extent      = extent;
    m_format      = format;
    m_mipLevels   = mipLevels;
    m_arrayLayers = arrayLayers;
    m_tiling      = tiling;
    m_usage       = usage;
    m_properties  = properties;

    if (externalHandle != VK_NULL_HANDLE)
    {
        m_handle = externalHandle;
        m_isSwapchainImage = true;
        return;
    }

    // Image Create Info
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.format        = m_format;
    imageInfo.extent        = m_extent;
    imageInfo.mipLevels     = m_mipLevels;
    imageInfo.arrayLayers   = m_arrayLayers;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling        = m_tiling;
    imageInfo.usage         = m_usage;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    result = vkCreateImage(vkDevice, &imageInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateImage' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Image.");
    }

    // Query Requirements
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(
        vkDevice, 
        m_handle,
        &memRequirements
    );

    // Allocate and Bind
    m_allocationHandle = m_allocator.allocate(
        memRequirements.size,
        memRequirements.memoryTypeBits,
        properties
    );
    m_allocator.bindImage(m_handle, m_allocationHandle);
}

void Image::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();
    
    // Destroy Image Handle
    if (!m_isSwapchainImage && m_handle != VK_NULL_HANDLE)
    {
        vkDestroyImage(vkDevice, m_handle, nullptr);
    }
    m_handle = VK_NULL_HANDLE;
    
    // Destroy Allocation Handle
    if (m_allocationHandle != VK_NULL_HANDLE)
    {
        m_allocator.free(m_allocationHandle);
        m_allocationHandle = nullptr;
    }
}

void Image::transitionLayout(
    VkCommandBuffer    commandBuffer,
    VkImageLayout      oldLayout,
    VkImageLayout      newLayout,
    VkImageAspectFlags aspectMask,
    uint32_t mipLevels,
    uint32_t arrayLayers
) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_handle;
    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = arrayLayers;
    
    VkPipelineStageFlags srcStage  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkPipelineStageFlags destStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStage, destStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}
