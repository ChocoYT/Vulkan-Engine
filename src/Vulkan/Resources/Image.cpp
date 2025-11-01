#include "Vulkan/Resources/Image.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Resources/MemoryAllocator.hpp"

VulkanImage::VulkanImage(
    const VulkanDevice    &device,
    VulkanMemoryAllocator &allocator,
    VkImage                handle,
    VulkanAllocationHandle allocationHandle,
    VkExtent3D             extent,
    VkFormat               format,
    uint32_t               mipLevels,
    uint32_t               arrayLayers,
    VkImageTiling          tiling,
    VkImageUsageFlags      usage,
    VkMemoryPropertyFlags  properties,
    bool isSwapchainImage
) : m_device(device),
    m_allocator(allocator),
    m_handle(handle),
    m_extent(extent),
    m_format(format),
    m_mipLevels(mipLevels),
    m_arrayLayers(arrayLayers),
    m_tiling(tiling),
    m_usage(usage),
    m_properties(properties),
    m_isSwapchainImage(isSwapchainImage)
{}

VulkanImage::~VulkanImage()
{
    Cleanup();
}

std::unique_ptr<VulkanImage> VulkanImage::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkExtent3D            extent,
    VkFormat              format,
    uint32_t              mipLevels,
    uint32_t              arrayLayers,
    VkImageTiling         tiling,
    VkImageUsageFlags     usage,
    VkMemoryPropertyFlags properties,
    VkImage               externalHandle
) {
    VkImage                handle           = VK_NULL_HANDLE;
    VulkanAllocationHandle allocationHandle = nullptr;

    bool isSwapchainImage = false;

    if (externalHandle != VK_NULL_HANDLE)
    {
        handle = externalHandle;
        isSwapchainImage = true;
    }
    else
    {
        VkResult result = VK_SUCCESS;

        // Image Create Info
        VkImageCreateInfo imageInfo{};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.format        = format;
        imageInfo.extent        = extent;
        imageInfo.mipLevels     = mipLevels;
        imageInfo.arrayLayers   = arrayLayers;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling        = tiling;
        imageInfo.usage         = usage;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // Create Image
        result = vkCreateImage(device.GetHandle(), &imageInfo, nullptr, &handle);
        if (result != VK_SUCCESS)
        {
            std::cerr << "[ERROR]\t'vkCreateImage' Failed with Error Code " << result << "\n";

            throw std::runtime_error("Failed to Create Image.");
        }

        // Query Requirements
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(
            device.GetHandle(), 
            handle,
            &memRequirements
        );

        // Allocate and Bind
        allocationHandle = allocator.Allocate(
            physicalDevice,
            device,
            memRequirements.size,
            memRequirements.memoryTypeBits,
            properties
        );
        allocator.BindImage(device, handle, allocationHandle);
    }

    return std::unique_ptr<VulkanImage>(
        new VulkanImage(
            device,
            allocator,
            handle,
            allocationHandle,
            extent,
            format,
            mipLevels,
            arrayLayers,
            tiling,
            usage,
            properties,
            isSwapchainImage
        )
    );
}

void VulkanImage::Cleanup()
{
    // Destroy Allocation Handle
    if (m_allocationHandle != VK_NULL_HANDLE)
    {
        m_allocator.Free(m_device, m_allocationHandle);
        m_allocationHandle = nullptr;
    }

    // Destroy Image Handle
    if (!m_isSwapchainImage && m_handle != VK_NULL_HANDLE)
    {
        vkDestroyImage(m_device.GetHandle(), m_handle, nullptr);
    }
    m_handle = VK_NULL_HANDLE;
}

void VulkanImage::TransitionLayout(
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

VulkanImage::VulkanImage(VulkanImage &&other) noexcept : 
    m_device(other.m_device),
    m_allocator(other.m_allocator),
    m_handle(other.m_handle),
    m_allocationHandle(other.m_allocationHandle),
    m_extent(other.m_extent),
    m_format(other.m_format),
    m_mipLevels(other.m_mipLevels),
    m_arrayLayers(other.m_arrayLayers),
    m_tiling(other.m_tiling),
    m_usage(other.m_usage),
    m_properties(other.m_properties)
{
    other.m_handle           = VK_NULL_HANDLE;
    other.m_allocationHandle = nullptr;
    other.m_extent           = VkExtent3D{0, 0, 0};
    other.m_format           = VK_FORMAT_UNDEFINED;
    other.m_mipLevels        = 1;
    other.m_arrayLayers      = 1;
    other.m_tiling           = VK_IMAGE_TILING_OPTIMAL;
    other.m_usage            = 0;
    other.m_properties       = 0;
}

VulkanImage& VulkanImage::operator=(VulkanImage &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle             = other.m_handle;
        m_allocationHandle   = other.m_allocationHandle;
        m_extent             = other.m_extent;
        m_format             = other.m_format;
        m_mipLevels          = other.m_mipLevels;
        m_arrayLayers        = other.m_arrayLayers;
        m_tiling             = other.m_tiling;
        m_usage              = other.m_usage;
        m_properties         = other.m_properties;

        other.m_handle           = VK_NULL_HANDLE;
        other.m_allocationHandle = nullptr;
        other.m_extent           = VkExtent3D{0, 0, 0};
        other.m_format           = VK_FORMAT_UNDEFINED;
        other.m_mipLevels        = 1;
        other.m_arrayLayers      = 1;
        other.m_tiling           = VK_IMAGE_TILING_OPTIMAL;
        other.m_usage            = 0;
        other.m_properties       = 0;
    }

    return *this;
}
