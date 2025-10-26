#include "Vulkan/Synchronization/Fence.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

Fence::Fence(const Context &context) : m_context(context) {}
Fence::~Fence()
{
    cleanup();
}

void Fence::init(VkFenceCreateFlags flags)
{
    VkDevice device = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;

    VkFence fence = VK_NULL_HANDLE;

    result = vkCreateFence(device, &fenceInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFence' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Fence.");
    }

    std::cout << "[INFO]\tFence Created Successfully.\n";
}

void Fence::cleanup()
{
    VkDevice device = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyFence(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void Fence::wait()
{
    VkDevice device = m_context.getDevice().getHandle();
    
    vkWaitForFences(device, 1, &m_handle, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &m_handle);
}