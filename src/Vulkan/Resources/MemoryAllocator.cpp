#include "Vulkan/Resources/MemoryAllocator.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"

#include <iostream>
#include <stdexcept>

struct MemoryAllocation
{
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize   offset = 0;
    VkDeviceSize   size   = 0;
    void* pMappedData     = nullptr;
};

MemoryAllocator::MemoryAllocator(const Context &context) : m_context(context) {}
MemoryAllocator::~MemoryAllocator() = default;

void MemoryAllocator::free(AllocationHandle handle)
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    MemoryAllocation* allocation = static_cast<MemoryAllocation*>(handle);

    if (allocation == nullptr) return;
    
    if (allocation->pMappedData != nullptr) {
        unmap(allocation);
        allocation->pMappedData = nullptr;
    }

    if (allocation->memory != VK_NULL_HANDLE) {
        vkFreeMemory(vkDevice, allocation->memory, nullptr);
        allocation->memory = VK_NULL_HANDLE;
    }
    
    delete allocation;
}

AllocationHandle MemoryAllocator::allocate(
    VkDeviceSize          size,
    uint32_t              memoryTypeBits,
    VkMemoryPropertyFlags properties
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;
    
    uint32_t memoryTypeIndex = findMemoryType(memoryTypeBits, properties);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    VkDeviceMemory memoryHandle = VK_NULL_HANDLE;

    result = vkAllocateMemory(vkDevice, &allocInfo, nullptr, &memoryHandle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateMemory' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Memory.");
    }

    MemoryAllocation* allocation = new MemoryAllocation{};
    allocation->memory = memoryHandle;
    allocation->size   = size;
    allocation->offset = 0;

    return static_cast<AllocationHandle>(allocation);
}

void* MemoryAllocator::map(AllocationHandle handle)
{
    MemoryAllocation* allocation = static_cast<MemoryAllocation*>(handle);

    if (allocation == nullptr) return nullptr;
    
    if (allocation->pMappedData)
        return allocation->pMappedData;

    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    void* hostData = nullptr;
    
    result = vkMapMemory(
        vkDevice,
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

void MemoryAllocator::unmap(AllocationHandle handle)
{
    MemoryAllocation* allocation = static_cast<MemoryAllocation*>(handle);

    if (allocation              == nullptr) return;
    if (allocation->pMappedData == nullptr) return;

    VkDevice vkDevice = m_context.getDevice().getHandle();

    vkUnmapMemory(vkDevice, allocation->memory);

    allocation->pMappedData = nullptr;
}

void MemoryAllocator::bindBuffer(
    VkBuffer         vkBuffer, 
    AllocationHandle handle
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    vkBindBufferMemory(vkDevice, vkBuffer, static_cast<MemoryAllocation*>(handle)->memory, 0);
}

void MemoryAllocator::bindImage(
    VkImage          vkImage, 
    AllocationHandle handle
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    vkBindImageMemory(vkDevice, vkImage, static_cast<MemoryAllocation*>(handle)->memory, 0);
}

uint32_t MemoryAllocator::findMemoryType(
    uint32_t              typeFilter, 
    VkMemoryPropertyFlags properties
) const {
    VkPhysicalDevice vkPhysicalDevice = m_context.getPhysicalDevice().getHandle();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && 
            ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
            return i;
    }
    
    throw std::runtime_error("Failed to Find Suitable Memory Type.");
}
