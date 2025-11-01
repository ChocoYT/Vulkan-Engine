#pragma once

#include <memory>

#include <vulkan/vulkan.h>

using VulkanAllocationHandle = void*;

class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;

class VulkanImage
{
    public:
        ~VulkanImage();

        static std::unique_ptr<VulkanImage> Create(
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
            VkImage               externalHandle = VK_NULL_HANDLE
        );

        void TransitionLayout(
            VkCommandBuffer    commandBuffer,
            VkImageLayout      oldLayout,
            VkImageLayout      newLayout,
            VkImageAspectFlags aspectMask,
            uint32_t mipLevels   = 1,
            uint32_t arrayLayers = 1
        );

        const VkImage                GetHandle()           const { return m_handle;           }
        const VulkanAllocationHandle GetAllocationHandle() const { return m_allocationHandle; }

        VkExtent3D            GetExtent()      const { return m_extent; }
        VkFormat              GetFormat()      const { return m_format; }
        uint32_t              GetMipLevels()   const { return m_mipLevels; }
        uint32_t              GetArrayLayers() const { return m_arrayLayers; }
        VkImageTiling         GetTiling()      const { return m_tiling; }
        VkImageUsageFlags     GetUsage()       const { return m_usage; }
        VkMemoryPropertyFlags GetProperties()  const { return m_properties; }

        bool IsSwapchainImage() const { return m_isSwapchainImage; }
        
    private:
        VulkanImage(
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
        );

        // Remove Copying Semantics
        VulkanImage(const VulkanImage&) = delete;
        VulkanImage& operator=(const VulkanImage&) = delete;
        
        // Safe Move Semantics
        VulkanImage(VulkanImage &&other) noexcept;
        VulkanImage& operator=(VulkanImage &&other) noexcept;

        void Cleanup();

        const VulkanDevice    &m_device;
        VulkanMemoryAllocator &m_allocator;

        VkImage                m_handle           = VK_NULL_HANDLE;
        VulkanAllocationHandle m_allocationHandle = nullptr;

        VkExtent3D            m_extent{0, 0, 0};
        VkFormat              m_format      = VK_FORMAT_UNDEFINED;
        uint32_t              m_mipLevels   = 1;
        uint32_t              m_arrayLayers = 1;
        VkImageTiling         m_tiling      = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags     m_usage       = 0;
        VkMemoryPropertyFlags m_properties  = 0;

        bool m_isSwapchainImage = false;
};
