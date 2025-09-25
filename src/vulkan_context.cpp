#include "vulkan_context.hpp"

void VulkanContext::init(const Window &window)
{
    createInstance();
    createSurface(window.getHandle());
}

void VulkanContext::createInstance()
{
    // Application Info
    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = SCREEN_NAME.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = ENGINE_NAME.c_str();
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_4;

    // Create Info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Get GLFW Extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount   = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount       = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create Vulkan Instance.");
    }
    std::cout << "[INFO]\tVulkan Instance created successfully." << std::endl;

    // Get Vulkan Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "[INFO]\tAvailable Extensions:\n";

    for (const auto& extension : extensions) {
        std::cout << "[INFO]\t  " << extension.extensionName << '\n';
    }
}

void VulkanContext::createSurface(GLFWwindow* windowHandle)
{
    VkResult result = glfwCreateWindowSurface(instance, windowHandle, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan surface.");
    }
    std::cout << "[INFO]\tVulkan surface created successfully.\n";
}

void VulkanContext::cleanup()
{
    vkDestroyInstance(instance, nullptr);
}