#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "vulkan_context.hpp"

class VulkanBuffer
{
    public:
        void cleanup();

        void writeData(const void* srcData, VkDeviceSize dataSize);
        void copyTo(VulkanBuffer &dst);

        // Buffer Getters
        VkBuffer     getBuffer() const { return buffer; }
        VkDeviceSize getSize()   const { return size;   }

        // Static Constructors
        void initVertexBuffer(VulkanContext &vulkanContext, void *bufferData, VkDeviceSize bufferSize);
        void initIndexBuffer(VulkanContext &vulkanContext, void *bufferData, VkDeviceSize bufferSize);
        static std::vector<VulkanBuffer> createUniformBuffers(VulkanContext &vulkanContext, void *bufferData, VkDeviceSize bufferSize);

    private:
        VkBuffer       buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDeviceSize   size   = 0;

        void createBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void init(VulkanContext &vulkanContext, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        void initStagingBuffer(VulkanContext &vulkanContext, VkDeviceSize bufferSize);

        VulkanContext *context;
};
