#include "Vulkan/Swapchain/Framebuffer.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/RenderPass/RenderPass.hpp"

Framebuffer::Framebuffer(
    const Context    &context,
    const RenderPass &renderPass
) : m_context(context), m_renderPass(renderPass) {}

Framebuffer::~Framebuffer()
{
    cleanup();
}

void Framebuffer::init(
    const std::vector<VkImageView> &attachments,
    VkExtent2D extent
) {
    VkDevice     vkDevice     = m_context.getDevice().getHandle();
    VkRenderPass vkRenderPass = m_renderPass.getHandle();

    VkResult result = VK_SUCCESS;

    std::vector<VkImageView> vkAttachments;
    for (size_t i = 0; i < attachments.size(); ++i)
    {
        vkAttachments.emplace_back(attachments[i]);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = vkRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = vkAttachments.data();
    framebufferInfo.width           = extent.width;
    framebufferInfo.height          = extent.height;
    framebufferInfo.layers          = 1;

    result = vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFramebuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Framebuffers.");
    }

    std::cout << "[INFO]\tSwapchain Framebuffers Created Successfully.\n";
}

void Framebuffer::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
