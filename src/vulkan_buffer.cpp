#include "vulkan_buffer.hpp"

void VulkanBuffer::init(VulkanContext &vulkanContext, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    size    = bufferSize;
    context = &vulkanContext;

    createBuffer(usage, properties);
}

void VulkanBuffer::cleanup()
{
    // Destroy Buffer
    if (buffer != VK_NULL_HANDLE)
        vkDestroyBuffer(context->getDevice(), buffer, nullptr);
        
    // Destroy and Free Memory
    if (memory != VK_NULL_HANDLE)
        vkFreeMemory(context->getDevice(), memory, nullptr);
}

void VulkanBuffer::createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkResult result = VK_SUCCESS;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size  = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer(context->getDevice(), &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Buffer.");
    }

    // Query Requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context->getDevice(), buffer, &memRequirements);

    // Allocate
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(context->getDevice(), &allocInfo, nullptr, &memory);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateMemory' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(context->getDevice(), buffer, memory, 0);
}

void VulkanBuffer::copyData(const void* srcData, VkDeviceSize dataSize)
{
    void* dst;
    vkMapMemory(context->getDevice(), memory, 0, dataSize, 0, &dst);
    std::memcpy(dst, srcData, static_cast<size_t>(dataSize));
    vkUnmapMemory(context->getDevice(), memory);
}

void VulkanBuffer::copyTo(VulkanBuffer &dst)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = context->getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(context->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = std::min(size, dst.getSize());
    vkCmdCopyBuffer(commandBuffer, buffer, dst.getBuffer(), 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(context->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(context->getGraphicsQueue());

    vkFreeCommandBuffers(context->getDevice(), context->getCommandPool(), 1, &commandBuffer);
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("Failed to Find Suitable Memory Type.");
}
