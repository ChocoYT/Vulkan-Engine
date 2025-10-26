#include "Vulkan/Commands/CommandPool.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

CommandPool::CommandPool(const Context &context) : m_context(context) {}
CommandPool::~CommandPool()
{
    cleanup();
}

void CommandPool::init()
{
    VkDevice vkDevice            = m_context.getDevice().getHandle();
    uint32_t graphicsQueueFamily = m_context.getPhysicalDevice().getGraphicsQueueFamily();

    VkResult result = VK_SUCCESS;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateCommandPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Command Pool.");
    }

    std::cout << "[INFO]\tCommand Pool Created Successfully.\n";
}

void CommandPool::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    // Free Command Buffers
    if (!m_commandBuffers.empty())
    {
        vkFreeCommandBuffers(
            vkDevice,
            m_handle,
            static_cast<uint32_t>(m_commandBuffers.size()),
            m_commandBuffers.data()
        );
        m_commandBuffers.clear();
    }
    
    // Destroy Command Pool
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void CommandPool::createCommandBuffers(uint32_t frameCount)
{
    VkDevice device = m_context.getDevice().getHandle();

    m_commandBuffers.resize(frameCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_handle;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = frameCount;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, m_commandBuffers.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateCommandBuffers' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Command Buffers.");
    }

    std::cout << "[INFO]\tCommand Buffers Allocated Successfully.\n";
}

VkCommandBuffer CommandPool::beginFrame(uint32_t currentFrame)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer commandBuffer = m_commandBuffers[currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkBeginCommandBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Begin Command Buffer.");
    }

    return commandBuffer;
}
