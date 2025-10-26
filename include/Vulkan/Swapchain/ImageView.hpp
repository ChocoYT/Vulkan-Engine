#pragma once

#include <vulkan/vulkan.h>

class Context;

class ImageView
{
    public:
        ImageView(const Context &context);
        ~ImageView();

        void init(
            VkImage  image,
            VkFormat format
        );
        void cleanup();

        const VkImageView getHandle() const { return m_handle; }

    private:
        VkImageView m_handle = VK_NULL_HANDLE;

        const Context &m_context;
};
