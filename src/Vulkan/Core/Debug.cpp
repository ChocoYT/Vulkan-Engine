#include "Vulkan/Core/Debug.hpp"

#include "Vulkan/Core/Context.hpp"
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

Debug::Debug(const Context &context) : m_context(context) {}
Debug::~Debug()
{
    cleanup();
}

void Debug::init()
{
    VkInstance instance = m_context.getInstance().getHandle();

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
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (!funcCreateDebugMessenger)
        throw std::runtime_error("Could not load vkCreateDebugUtilsMessengerEXT");
    if (funcCreateDebugMessenger(instance, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan Debug Callback.");

    std::cout << "[INFO]\tDebug Messenger Created Successfully\n";
}

void Debug::cleanup()
{
    VkInstance instance = m_context.getInstance().getHandle();

    // Destroy Debug Messenger
    auto funcDestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (funcDestroyDebugMessenger && m_handle != VK_NULL_HANDLE)
        funcDestroyDebugMessenger(instance, m_handle, nullptr);
}