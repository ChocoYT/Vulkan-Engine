#include "Vulkan/Pipeline/ShaderModule.hpp"

#include <iostream>
#include <stdexcept>

#include "Utils.hpp"

#include "Vulkan/Core/Device.hpp"

VulkanShaderModule::VulkanShaderModule(
    const VulkanDevice &device,
    VkShaderModule m_handle
) : m_device(device),
    m_handle(m_handle)
{}

VulkanShaderModule::~VulkanShaderModule()
{
    Cleanup();
}

std::unique_ptr<VulkanShaderModule> VulkanShaderModule::Create(
    const VulkanDevice &device,
    const std::string &filePath
) {
    VkResult result = VK_SUCCESS;

    std::vector<char> code = readFileBinary(filePath);

    // Shader Module Create Info
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    // Create Shader Module
    VkShaderModule handle = VK_NULL_HANDLE;
    result = vkCreateShaderModule(device.GetHandle(), &createInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateShaderModule' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Shader Module.");
    }

    std::cout << "[INFO]\tShader Module Created Successfully.\n";

    return std::unique_ptr<VulkanShaderModule>(
        new VulkanShaderModule(
            device,
            handle
        )
    );
}

void VulkanShaderModule::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanShaderModule::VulkanShaderModule(VulkanShaderModule &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanShaderModule& VulkanShaderModule::operator=(VulkanShaderModule &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}
