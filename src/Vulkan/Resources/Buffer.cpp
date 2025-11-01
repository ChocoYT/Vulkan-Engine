#include "Vulkan/Resources/Buffer.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Commands/CommandPool.hpp"
#include "Vulkan/Resources/MemoryAllocator.hpp"

VulkanBuffer::VulkanBuffer(
    const VulkanDevice     &device,
    VulkanMemoryAllocator  &allocator,
    VkBuffer               handle,
    VulkanAllocationHandle allocationHandle,
    VkDeviceSize           size,
    VkBufferUsageFlags     usage,
    VkMemoryPropertyFlags  properties
) : m_device(device),
    m_allocator(allocator),
    m_handle(handle),
    m_allocationHandle(allocationHandle),
    m_size(size),
    m_usage(usage),
    m_properties(properties)
{}

VulkanBuffer::~VulkanBuffer()
{
    Cleanup();
}

std::unique_ptr<VulkanBuffer> VulkanBuffer::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize          size,
    VkBufferUsageFlags    usage,
    VkMemoryPropertyFlags properties
) {
    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    // Create Buffer
    VkBuffer handle;
    result = vkCreateBuffer(device.GetHandle(), &bufferInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Buffer.");
    }

    // Query Requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.GetHandle(), handle, &memRequirements);
    
    // Allocate and Bind
    VulkanAllocationHandle allocationHandle = allocator.Allocate(
        physicalDevice,
        device,
        memRequirements.size,
        memRequirements.memoryTypeBits,
        properties
    );
    allocator.BindBuffer(device, handle, allocationHandle);

    return std::unique_ptr<VulkanBuffer>(
        new VulkanBuffer(
            device,
            allocator,
            handle,
            allocationHandle,
            size,
            usage,
            properties
        )
    );
}

void VulkanBuffer::Update(
    const void   *data,
    VkDeviceSize size
) {
    if (size > m_size)
        throw std::runtime_error("Buffer cannot be updated because 'dataSize' is larger than the buffer's size.");

    void *dstData = m_allocator.Map(m_device, m_allocationHandle);

    std::memcpy(dstData, data, static_cast<size_t>(size));
    
    m_allocator.Unmap(m_device, m_allocationHandle);
}

void VulkanBuffer::CopyTo(
    const VulkanCommandPool &commandPool,
    const VulkanBuffer      &dst
) {
    if (dst.GetSize() < m_size)
        throw std::runtime_error("Buffer cannot be copied because the original buffer's size is smaller than 'dst.size'.");

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = commandPool.GetHandle();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device.GetHandle(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = m_size;
    vkCmdCopyBuffer(commandBuffer, m_handle, dst.GetHandle(), 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_device.GetGraphicsQueue());

    vkFreeCommandBuffers(m_device.GetHandle(), commandPool.GetHandle(), 1, &commandBuffer);
}

void VulkanBuffer::Cleanup()
{
    // Destroy Allocation Handle
    if (m_allocationHandle != nullptr)
    {
        m_allocator.Free(m_device, m_allocationHandle);
        m_allocationHandle = nullptr;
    }

    // Destroy Buffer
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanBuffer::VulkanBuffer(VulkanBuffer &&other) noexcept : 
    m_device(other.m_device),
    m_allocator(other.m_allocator),
    m_handle(other.m_handle),
    m_allocationHandle(other.m_allocationHandle),
    m_size(other.m_size),
    m_usage(other.m_usage),
    m_properties(other.m_properties)
{
    other.m_handle           = VK_NULL_HANDLE;
    other.m_allocationHandle = nullptr;
    other.m_size             = 0;
    other.m_usage            = 0;
    other.m_properties       = 0;
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle             = other.m_handle;
        m_allocationHandle   = other.m_allocationHandle;
        m_size               = other.m_size;
        m_usage              = other.m_usage;
        m_properties         = other.m_properties;

        other.m_handle           = VK_NULL_HANDLE;
        other.m_allocationHandle = nullptr;
        other.m_size             = 0;
        other.m_usage            = 0;
        other.m_properties       = 0;
    }

    return *this;
}
