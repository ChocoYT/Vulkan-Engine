#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

using VulkanAllocationHandle = void*;

class VulkanPhysicalDevice;
class VulkanDevice;

class VulkanMemoryAllocator
{
    public:
        ~VulkanMemoryAllocator();

        static std::unique_ptr<VulkanMemoryAllocator> Create();

        void Free(
            const VulkanDevice     &device,
            VulkanAllocationHandle handle
        );

        VulkanAllocationHandle Allocate(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            VkDeviceSize          size,
            uint32_t              memoryTypeBits,
            VkMemoryPropertyFlags properties
        );

        void* Map(
            const VulkanDevice     &device,
            VulkanAllocationHandle allocationHandle
        );
        void Unmap(
            const VulkanDevice     &device,
            VulkanAllocationHandle allocationHandle
        );

        void BindBuffer(
            const VulkanDevice     &device,
            VkBuffer               handle, 
            VulkanAllocationHandle allocationHandle
        );
        void BindImage(
            const VulkanDevice     &device,
            VkImage                handle,
            VulkanAllocationHandle allocationHandle
        );

    private:
        VulkanMemoryAllocator() = default;;

        uint32_t FindMemoryType(
            const VulkanPhysicalDevice &physicalDevice,
            uint32_t              typeFilter, 
            VkMemoryPropertyFlags properties
        ) const;
};
