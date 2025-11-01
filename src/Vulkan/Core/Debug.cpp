#include "Vulkan/Core/Debug.hpp"

#include <iostream>

#include "Vulkan/Core/Instance.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    std::cerr << "[VULKAN VALIDATION]\t" << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

VulkanDebug::VulkanDebug(
    const VulkanInstance     &instance,
    VkDebugUtilsMessengerEXT handle
) : m_instance(instance),
    m_handle(handle)
{}

VulkanDebug::~VulkanDebug()
{
    Cleanup();
}

std::unique_ptr<VulkanDebug> VulkanDebug::Create(
    const VulkanInstance &instance
) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    // Load Extensions
    auto funcCreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance.GetHandle(), "vkCreateDebugUtilsMessengerEXT");

    // Create Debug Messenger
    VkDebugUtilsMessengerEXT handle = VK_NULL_HANDLE;
    
    if (!funcCreateDebugMessenger)
        throw std::runtime_error("Could not load vkCreateDebugUtilsMessengerEXT");
    if (funcCreateDebugMessenger(instance.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan Debug Callback.");

    std::cout << "[INFO]\tDebug Messenger Created Successfully\n";

    return std::unique_ptr<VulkanDebug>(
        new VulkanDebug(
            instance,
            handle
        )
    );
}

void VulkanDebug::Cleanup()
{
    // Load Extensions
    auto funcDestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(m_instance.GetHandle(), "vkDestroyDebugUtilsMessengerEXT");

    // Destroy Debug Messenger
    if (m_handle != VK_NULL_HANDLE)
    {
        if (funcDestroyDebugMessenger)
            funcDestroyDebugMessenger(m_instance.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanDebug::VulkanDebug(VulkanDebug &&other) noexcept : 
    m_instance(other.m_instance),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanDebug& VulkanDebug::operator=(VulkanDebug &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}