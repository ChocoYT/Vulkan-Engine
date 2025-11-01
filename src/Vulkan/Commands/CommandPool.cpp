#include "Vulkan/Commands/CommandPool.hpp"

#include <iostream>

#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

VulkanCommandPool::VulkanCommandPool(
    const VulkanDevice &device,
    VkCommandPool handle
) : m_device(device),
    m_handle(handle)
{}

VulkanCommandPool::~VulkanCommandPool()
{
    Cleanup();
}

std::unique_ptr<VulkanCommandPool> VulkanCommandPool::Create(
    const VulkanDevice &device
) {
    VkResult result = VK_SUCCESS;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = device.GetGraphicsQueueFamily();
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    // Create Command Pool
    VkCommandPool handle = VK_NULL_HANDLE;
    result = vkCreateCommandPool(device.GetHandle(), &poolInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateCommandPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Command Pool.");
    }

    std::cout << "[INFO]\tCommand Pool Created Successfully.\n";

    return std::unique_ptr<VulkanCommandPool>(
        new VulkanCommandPool(
            device,
            handle
        )
    );
}

void VulkanCommandPool::Cleanup()
{
    // Free Command Buffers
    if (!m_commandBuffers.empty())
    {
        vkFreeCommandBuffers(
            m_device.GetHandle(),
            m_handle,
            static_cast<uint32_t>(m_commandBuffers.size()),
            m_commandBuffers.data()
        );
        m_commandBuffers.clear();
    }
    
    // Destroy Command Pool
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void VulkanCommandPool::CreateCommandBuffers(uint32_t frameCount)
{
    m_commandBuffers.resize(frameCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_handle;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = frameCount;

    VkResult result = vkAllocateCommandBuffers(m_device.GetHandle(), &allocInfo, m_commandBuffers.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateCommandBuffers' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Command Buffers.");
    }

    std::cout << "[INFO]\tCommand Buffers Allocated Successfully.\n";
}

VkCommandBuffer VulkanCommandPool::BeginFrame(uint32_t currentFrame)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer commandBuffer = m_commandBuffers[currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkBeginCommandBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Begin Command Buffer.");
    }

    return commandBuffer;
}

VulkanCommandPool::VulkanCommandPool(VulkanCommandPool &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle),
    m_commandBuffers(std::move(other.m_commandBuffers))
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_commandBuffers.clear();
}

VulkanCommandPool& VulkanCommandPool::operator=(VulkanCommandPool &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle         = other.m_handle;
        m_commandBuffers = std::move(other.m_commandBuffers);

        other.m_handle = VK_NULL_HANDLE;
        other.m_commandBuffers.clear();
    }

    return *this;
}
