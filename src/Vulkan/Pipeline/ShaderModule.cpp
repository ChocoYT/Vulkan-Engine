#include "Vulkan/Pipeline/ShaderModule.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

ShaderModule::ShaderModule(const Context &context) : m_context(context) {}
ShaderModule::~ShaderModule()
{
    cleanup();
}

void ShaderModule::init(const std::string &filePath)
{
    VkDevice device = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    std::vector<char> code = readFileBinary(filePath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    result = vkCreateShaderModule(device, &createInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateShaderModule' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Shader Module.");
    }

    std::cout << "[INFO]\tShader Module Created Successfully.\n";
}

void ShaderModule::cleanup()
{
    VkDevice device = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
