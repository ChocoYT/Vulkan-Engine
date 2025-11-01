#include "Vulkan/RenderPass/RenderPass.hpp"

#include <iostream>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Swapchain/Swapchain.hpp"

VulkanRenderPass::VulkanRenderPass(
    const VulkanDevice &device,
    VkRenderPass handle
) : m_device(device),
    m_handle(handle)
{}

VulkanRenderPass::~VulkanRenderPass()
{
    Cleanup();
}

std::unique_ptr<VulkanRenderPass> VulkanRenderPass::Create(
    const VulkanDevice    &device,
    const VulkanSwapchain &swapchain
) {
    VkResult result = VK_SUCCESS;

    // Color Attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = swapchain.GetFormat();
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

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    // Dependency
    VkSubpassDependency dependency{};
    dependency.srcSubpass    = 0; 
    dependency.dstSubpass    = VK_SUBPASS_EXTERNAL; 
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    // Create Info
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency; 

    // Create Render Pass
    VkRenderPass handle = VK_NULL_HANDLE;
    result = vkCreateRenderPass(device.GetHandle(), &renderPassInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateRenderPass' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Render Pass.");
    }

    std::cout << "[INFO]\tRender Pass Created Successfully.\n";

    return std::unique_ptr<VulkanRenderPass>(
        new VulkanRenderPass(
            device,
            handle
        )
    );
}

void VulkanRenderPass::Cleanup()
{
    // Destroy Render Pass
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

VulkanRenderPass::VulkanRenderPass(VulkanRenderPass &&other) noexcept :
    m_device(other.m_device),
    m_handle(other.m_handle)
{
    other.m_handle = VK_NULL_HANDLE;
}

VulkanRenderPass& VulkanRenderPass::operator=(VulkanRenderPass &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();
        
        m_handle = other.m_handle;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}
