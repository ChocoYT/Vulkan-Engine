#include "vulkan_context.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    std::cerr << "[VULKAN VALIDATION]\t" << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

void VulkanContext::init(const Window &window)
{
    GLFWwindow* windowHandle = window.getHandle();
    
    createInstance();
    createDebugCallback();
    createSurface(windowHandle);

    pickPhysicalDevice();
    createDevice();

    createSwapchain(windowHandle);
    createRenderPass();
    createImageViews();
    createFramebuffers();
    
    createCommandPool();
    createCommandBuffers();
}

void VulkanContext::cleanup()
{
    // Destroy Command Pool
    if (commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(device, commandPool, nullptr);
    
    // Destroy Frame Buffers
    for (auto framebuffer : swapchainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);

    // Destroy Image Views
    for (auto imageView : swapchainImageViews)
        vkDestroyImageView(device, imageView, nullptr);

    // Destroy Render Pass
    if (renderPass != VK_NULL_HANDLE)
        vkDestroyRenderPass(device, renderPass, nullptr);

    // Destroy Swapchain
    if (swapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(device, swapchain, nullptr);

    // Destroy Device
    if (device != VK_NULL_HANDLE)
        vkDestroyDevice(device, nullptr);

    // Destroy Surface
    if (surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance, surface, nullptr);

    // Destroy Debug Messenger
    auto funcDestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (funcDestroyDebugMessenger && debugMessenger != VK_NULL_HANDLE)
        funcDestroyDebugMessenger(instance, debugMessenger, nullptr);

    // Destroy Instance
    if (instance != VK_NULL_HANDLE)
        vkDestroyInstance(instance, nullptr);
}

void VulkanContext::createInstance()
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
    result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateInstance' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Vulkan Instance.");
    }
    
    std::cout << "[INFO]\tVulkan Instance Created Successfully.\n";
}

void VulkanContext::createDebugCallback()
{
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
    if (funcCreateDebugMessenger(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan Debug Callback.");
}

void VulkanContext::createSurface(GLFWwindow* windowHandle)
{
    VkResult result = VK_SUCCESS;
    
    result = glfwCreateWindowSurface(instance, windowHandle, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'glfwCreateWindowSurface' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to create Vulkan Surface.");
    }
    
    std::cout << "[INFO]\tVulkan Surface Created Successfully.\n";
}

void VulkanContext::createSwapchain(GLFWwindow* windowHandle)
{
    VkResult result = VK_SUCCESS;

    int width, height;
    glfwGetFramebufferSize(windowHandle, &width, &height);

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities, width, height);

    uint32_t imageCount = FRAMES_IN_FLIGHT;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // Create Info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface          = surface;
    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };
    if (uniqueQueueFamilies.size() > 1)
    {
        std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices   = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;
    }

    createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;
    createInfo.oldSwapchain   = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSwapchainKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Swapchain.");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

    swapchainFormat = surfaceFormat.format;
    swapchainExtent = extent;

    std::cout << "[INFO]\tSwapchain Created Successfully with " << imageCount << " Images.\n";
}

void VulkanContext::createImageViews()
{
    VkResult result = VK_SUCCESS;

    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        result = vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "[ERROR]\t'vkCreateImageView' Failed with Error Code " << result << "\n";

            throw std::runtime_error("Failed to Create Image Views.");
        }
    }

    std::cout << "[INFO]\tSwapchain Image Views Created Successfully.\n";
}

void VulkanContext::createRenderPass()
{
    VkResult result = VK_SUCCESS;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = swapchainFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateRenderPass' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Render Pass.");
    }

    std::cout << "[INFO]\tRender Pass Created Successfully.\n";
}

void VulkanContext::createFramebuffers()
{
    VkResult result = VK_SUCCESS;

    swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); ++i)
    {
        VkImageView attachments[] = {
            swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = swapchainExtent.width;
        framebufferInfo.height          = swapchainExtent.height;
        framebufferInfo.layers          = 1;

        result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]);
        if (result != VK_SUCCESS)
        {
            std::cerr << "[ERROR]\t'vkCreateFramebuffer' Failed with Error Code " << result << "\n";

            throw std::runtime_error("Failed to Create Framebuffers.");
        }
    }

    std::cout << "[INFO]\tSwapchain Framebuffers Created Successfully.\n";
}

bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device)
{
    findQueueFamilies(device);

    return graphicsQueueFamily != UINT32_MAX && presentQueueFamily != UINT32_MAX;
}

void VulkanContext::findQueueFamilies(VkPhysicalDevice device)
{
    graphicsQueueFamily = UINT32_MAX;
    presentQueueFamily  = UINT32_MAX;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsQueueFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
            presentQueueFamily = i;

        if (graphicsQueueFamily != UINT32_MAX && presentQueueFamily != UINT32_MAX)
            break;
        
        ++i;
    }
}

void VulkanContext::pickPhysicalDevice()
{
    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0 || result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEnumeratePhysicalDevices' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto &deviceCandidate : devices)
    {
        if (isDeviceSuitable(deviceCandidate))
        {
            physicalDevice = deviceCandidate;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a Suitable GPU.");
    
    std::cout << "[INFO]\tPhysical Device Selected Successfully.\n";
}

void VulkanContext::createDevice()
{
    VkResult result = VK_SUCCESS;

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    // Extensions
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Create Info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos    = queueCreateInfos.data();
    createInfo.pEnabledFeatures     = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDevice' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Logical Device.");
    }

    // Get Handles
    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);

    std::cout << "[INFO]\tLogical Device Created Successfully.\n";
}

void VulkanContext::createCommandPool()
{
    VkResult result = VK_SUCCESS;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateCommandPool' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Command Pool.");
    }

    std::cout << "[INFO]\tCommand Pool Created Successfully.\n";
}

void VulkanContext::createCommandBuffers()
{
    commandBuffers.resize(FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = FRAMES_IN_FLIGHT;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateCommandBuffers' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to allocate command buffers.");
    }

    std::cout << "[INFO]\tCommand Buffers Allocated Successfully.\n";
}

VkCommandBuffer VulkanContext::beginFrame(int currentFrame)
{
    VkResult result = VK_SUCCESS;

    VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
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
