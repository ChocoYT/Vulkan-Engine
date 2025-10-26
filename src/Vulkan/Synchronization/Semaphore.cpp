#include "Vulkan/Synchronization/Semaphore.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

Semaphore::Semaphore(const Context &context) : m_context(context) {}
Semaphore::~Semaphore()
{
    cleanup();
}

void Semaphore::init(VkSemaphoreType type)
{
    VkDevice device = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    VkSemaphoreTypeCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.semaphoreType = type;
    semaphoreCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &semaphoreCreateInfo;
    createInfo.flags = 0;

    result = vkCreateSemaphore(device, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSemaphore' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Semaphore.");
    }

    std::cout << "[INFO]\tSemaphore Created Successfully.\n";
}

void Semaphore::cleanup()
{
    VkDevice device = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
