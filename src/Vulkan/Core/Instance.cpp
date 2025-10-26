#include "Vulkan/Core/Instance.hpp"

Instance::Instance() = default;
Instance::~Instance()
{
    cleanup();
}

void Instance::init()
{
    VkResult result = VK_SUCCESS;

    // Application Info
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = SCREEN_NAME.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = ENGINE_NAME.c_str();
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
    result = vkCreateInstance(&createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateInstance' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Vulkan Instance.");
    }
    
    std::cout << "[INFO]\tVulkan Instance Created Successfully.\n";
}

void Instance::cleanup()
{
    // Destroy Instance
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
