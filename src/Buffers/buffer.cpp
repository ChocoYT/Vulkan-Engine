#include "Buffers/buffer.hpp"

#include "Core/context.hpp"
#include "Core/physical_device.hpp"
#include "Core/device.hpp"
#include "Core/command_pool.hpp"

Buffer::Buffer(Context &context) : m_context(context) {}
Buffer::~Buffer()
{
    cleanup();
}

void Buffer::init(
    VkDeviceSize          bufferSize,
    VkBufferUsageFlags    usage,
    VkMemoryPropertyFlags properties
) {
    VkDevice device = m_context.getDevice().getHandle();

    m_size = bufferSize;

    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size  = m_size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    result = vkCreateBuffer(device, &bufferInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Buffer.");
    }

    // Query Requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_handle, &memRequirements);

    // Allocate
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits,
        properties
    );

    result = vkAllocateMemory(device, &allocInfo, nullptr, &m_memory);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateMemory' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, m_handle, m_memory, 0);
}

void Buffer::cleanup()
{
    VkDevice device = m_context.getDevice().getHandle();

    // Destroy Buffer
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
        
    // Destroy and Free Memory
    if (m_memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }
}

void Buffer::update(
    const void   *srcData,
    VkDeviceSize dataSize
) {
    VkDevice device = m_context.getDevice().getHandle();

    void* dst;
    vkMapMemory(device, m_memory, 0, dataSize, 0, &dst);
    std::memcpy(dst, srcData, static_cast<size_t>(dataSize));
    vkUnmapMemory(device, m_memory);
}

void Buffer::copyTo(const Buffer &dst)
{
    VkDevice      device      = m_context.getDevice().getHandle();
    VkCommandPool commandPool = m_context.getCommandPool().getHandle();

    VkQueue graphicsQueue = m_context.getDevice().getGraphicsQueue();

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = std::min(m_size, dst.getSize());
    vkCmdCopyBuffer(commandBuffer, m_handle, dst.getHandle(), 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

uint32_t Buffer::findMemoryType(
    uint32_t              typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDevice physicalDevice = m_context.getPhysicalDevice().getHandle();
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("Failed to Find Suitable Memory Type.");
}
