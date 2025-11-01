#include "Vulkan/Swapchain/Framebuffer.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/RenderPass/RenderPass.hpp"

VulkanFramebuffer::VulkanFramebuffer(
    const VulkanDevice &device,
    VkFramebuffer handle
) : m_device(device),
    m_handle(handle)
{}

VulkanFramebuffer::~VulkanFramebuffer()
{
    Cleanup();
}

std::unique_ptr<VulkanFramebuffer> VulkanFramebuffer::Create(
    const VulkanDevice             &device,
    const VulkanRenderPass         &renderPass,
    const std::vector<VkImageView> &attachments,
    VkExtent2D extent
) {
    VkResult result = VK_SUCCESS;

    std::vector<VkImageView> vkAttachments;
    for (size_t i = 0; i < attachments.size(); ++i)
    {
        vkAttachments.emplace_back(attachments[i]);
    }

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = renderPass.GetHandle();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = vkAttachments.data();
    framebufferInfo.width           = extent.width;
    framebufferInfo.height          = extent.height;
    framebufferInfo.layers          = 1;

    // Create Framebuffer
    VkFramebuffer handle = VK_NULL_HANDLE;
    result = vkCreateFramebuffer(device.GetHandle(), &framebufferInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFramebuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Framebuffers.");
    }

    std::cout << "[INFO]\tSwapchain Framebuffers Created Successfully.\n";

    return std::unique_ptr<VulkanFramebuffer>(
        new VulkanFramebuffer(
            device,
            handle
        )
    );
}

void VulkanFramebuffer::Cleanup()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanFramebuffer& VulkanFramebuffer::operator=(VulkanFramebuffer &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}
