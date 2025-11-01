#include "Vulkan/Core/Instance.hpp"

#include <iostream>
#include <vector>

#define GLFW_NO_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VulkanInstance::VulkanInstance(
    VkInstance handle
) : m_handle(handle)
{}

VulkanInstance::~VulkanInstance()
{
    Cleanup();
}

std::unique_ptr<VulkanInstance> VulkanInstance::Create(
    const std::string &appName,
    uint32_t          appVersion,
    const std::string &engineName,
    uint32_t          engineVersion
) {
    VkResult result = VK_SUCCESS;

    // Application Info
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = engineName.c_str();
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_3;

    // Create Info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get Vulkan Extensions
    uint32_t vulkanExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);

    std::vector<VkExtensionProperties> vulkanExtensions(vulkanExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, vulkanExtensions.data());

    std::cout << "[INFO]\tAvailable Extensions:\n";
    for (const auto &vulkanExtension : vulkanExtensions)
        std::cout << "[INFO]\t  " << vulkanExtension.extensionName << '\n';
    
    // Extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Layers
    const std::vector<const char*> layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    createInfo.enabledLayerCount   = static_cast<uint32_t>(layers.size());
    createInfo.ppEnabledLayerNames = layers.data();

    // Create Instance
    VkInstance handle = VK_NULL_HANDLE;
    result = vkCreateInstance(&createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateInstance' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Vulkan Instance.");
    }
    
    std::cout << "[INFO]\tVulkan Instance Created Successfully.\n";
    
    return std::unique_ptr<VulkanInstance>(
        new VulkanInstance(
            handle
        )
    );
}

void VulkanInstance::Cleanup()
{
    // Destroy Instance
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanInstance::VulkanInstance(VulkanInstance &&other) noexcept : 
    m_handle(other.m_handle)
{
    other = VulkanInstance{};
}

VulkanInstance& VulkanInstance::operator=(VulkanInstance &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other = VulkanInstance{};
    }

    return *this;
}
