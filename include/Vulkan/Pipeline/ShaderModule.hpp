#pragma once

#include <vulkan/vulkan.h>

#include <string>

class Context;

class ShaderModule
{
    public:
        ShaderModule(const Context &context);
        ~ShaderModule();

        void init(const std::string &filePath);
        void cleanup();

        const VkShaderModule getHandle() const { return m_handle; }

    private:
        VkShaderModule m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
