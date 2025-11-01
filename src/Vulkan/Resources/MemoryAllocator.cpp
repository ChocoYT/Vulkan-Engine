#include "Vulkan/Resources/MemoryAllocator.hpp"

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"

#include <iostream>
#include <stdexcept>

struct VulkanMemoryAllocation
{
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize   offset = 0;
    VkDeviceSize   size   = 0;
    void* pMappedData     = nullptr;
};

VulkanMemoryAllocator::~VulkanMemoryAllocator() = default;

std::unique_ptr<VulkanMemoryAllocator> VulkanMemoryAllocator::Create()
{
    return std::unique_ptr<VulkanMemoryAllocator>(
        new VulkanMemoryAllocator
    );
}

void VulkanMemoryAllocator::Free(
    const VulkanDevice     &device,
    VulkanAllocationHandle allocationHandle
) {
    VulkanMemoryAllocation* allocation = static_cast<VulkanMemoryAllocation*>(allocationHandle);

    if (allocation == nullptr) return;
    
    if (allocation->pMappedData != nullptr) {
        Unmap(device, allocation);
        allocation->pMappedData = nullptr;
    }

    if (allocation->memory != VK_NULL_HANDLE) {
        vkFreeMemory(device.GetHandle(), allocation->memory, nullptr);
        allocation->memory = VK_NULL_HANDLE;
    }
    
    delete allocation;
}

VulkanAllocationHandle VulkanMemoryAllocator::Allocate(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VkDeviceSize          size,
    uint32_t              memoryTypeBits,
    VkMemoryPropertyFlags properties
) {
    VkResult result = VK_SUCCESS;
    
    uint32_t memoryTypeIndex = FindMemoryType(physicalDevice, memoryTypeBits, properties);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    VkDeviceMemory memory = VK_NULL_HANDLE;
    result = vkAllocateMemory(device.GetHandle(), &allocInfo, nullptr, &memory);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateMemory' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Memory.");
    }

    VulkanMemoryAllocation* allocation = new VulkanMemoryAllocation{};
    allocation->memory = memory;
    allocation->size   = size;
    allocation->offset = 0;

    return static_cast<VulkanAllocationHandle>(allocation);
}

void* VulkanMemoryAllocator::Map(
    const VulkanDevice     &device,
    VulkanAllocationHandle allocationHandle
) {
    VulkanMemoryAllocation* allocation = static_cast<VulkanMemoryAllocation*>(allocationHandle);

    if (allocation == nullptr) return nullptr;
    
    if (allocation->pMappedData)
        return allocation->pMappedData;

    VkResult result = VK_SUCCESS;

    void* hostData = nullptr;
    
    result = vkMapMemory(
        device.GetHandle(),
        allocation->memory,
        allocation->offset,
        allocation->size,
        0,
        &hostData
    );
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkMapMemory' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Map Memory.");
    }

    allocation->pMappedData = hostData;
    
    return hostData;
}

void VulkanMemoryAllocator::Unmap(
    const VulkanDevice     &device,
    VulkanAllocationHandle allocationHandle
) {
    VulkanMemoryAllocation* allocation = static_cast<VulkanMemoryAllocation*>(allocationHandle);

    if (allocation              == nullptr) return;
    if (allocation->pMappedData == nullptr) return;

    vkUnmapMemory(device.GetHandle(), allocation->memory);

    allocation->pMappedData = nullptr;
}

void VulkanMemoryAllocator::BindBuffer(
    const VulkanDevice     &device,
    VkBuffer               handle, 
    VulkanAllocationHandle allocationHandle
) {
    vkBindBufferMemory(device.GetHandle(), handle, static_cast<VulkanMemoryAllocation*>(allocationHandle)->memory, 0);
}

void VulkanMemoryAllocator::BindImage(
    const VulkanDevice     &device,
    VkImage                handle, 
    VulkanAllocationHandle allocationHandle
) {
    vkBindImageMemory(device.GetHandle(), handle, static_cast<VulkanMemoryAllocation*>(allocationHandle)->memory, 0);
}

uint32_t VulkanMemoryAllocator::FindMemoryType(
    const VulkanPhysicalDevice &physicalDevice,
    uint32_t              typeFilter, 
    VkMemoryPropertyFlags properties
) const {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice.GetHandle(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && 
            ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
            return i;
    }
    
    throw std::runtime_error("Failed to Find Suitable Memory Type.");
}
