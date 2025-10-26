#include "Vulkan/Resources/Buffer.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Commands/CommandPool.hpp"

#include "Vulkan/Resources/MemoryAllocator.hpp"

Buffer::Buffer(
    const Context     &context,
    const CommandPool &commandPool,
    MemoryAllocator   &allocator
) : m_context(context), m_commandPool(commandPool), m_allocator(allocator) {}

Buffer::~Buffer()
{
    cleanup();
}

void Buffer::init(
    VkDeviceSize          bufferSize,
    VkBufferUsageFlags    usage,
    VkMemoryPropertyFlags properties
) {
    VkDevice vkDevice = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    m_size       = bufferSize;
    m_usage      = usage;
    m_properties = properties;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size  = m_size;
    bufferInfo.usage = m_usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    result = vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Buffer.");
    }

    // Query Requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDevice, m_handle, &memRequirements);
    
    // Allocate and Bind
    m_allocationHandle = m_allocator.allocate(
        memRequirements.size,
        memRequirements.memoryTypeBits,
        m_properties
    );
    m_allocator.bindBuffer(m_handle, m_allocationHandle);
}

void Buffer::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    // Destroy Buffer
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
    
    // Destroy Allocation Handle
    if (m_allocationHandle != VK_NULL_HANDLE)
    {
        m_allocator.free(m_allocationHandle);
        m_allocationHandle = nullptr;
    }
}

void Buffer::update(
    const void   *srcData,
    VkDeviceSize dataSize
) {
    if (dataSize > m_size)
        throw std::runtime_error("Buffer cannot be updated because 'dataSize' is larger than the buffer's size.");

    void *dstData = m_allocator.map(m_allocationHandle);

    std::memcpy(dstData, srcData, static_cast<size_t>(dataSize));
    
    m_allocator.unmap(m_allocationHandle);
}

void Buffer::copyTo(const Buffer &dst)
{
    if (dst.getSize() < m_size)
        throw std::runtime_error("Buffer cannot be copied because the original buffer's size is smaller than 'dst.size'.");

    VkDevice      device      = m_context.getDevice().getHandle();
    VkCommandPool commandPool = m_commandPool.getHandle();

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
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = m_size;
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
