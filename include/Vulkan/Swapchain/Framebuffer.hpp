#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class Context;
class RenderPass;

class Framebuffer
{
    public:
        Framebuffer(
            const Context    &context,
            const RenderPass &renderPass
        );
        ~Framebuffer();

        void init(
            const std::vector<VkImageView> &attachments,
            VkExtent2D extent
        );
        void cleanup();

        const VkFramebuffer getHandle() const { return m_handle; }

    private:
        VkFramebuffer m_handle = VK_NULL_HANDLE;

        const Context    &m_context;
        const RenderPass &m_renderPass;
};
