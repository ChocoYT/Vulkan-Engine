#include "Vulkan/RenderPass/RenderPass.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Swapchain/Swapchain.hpp"

RenderPass::RenderPass(
    const Context   &context,
    const Swapchain &swapchain
) : m_context(context), m_swapchain(swapchain) {}

RenderPass::~RenderPass()
{
    cleanup();
}

void RenderPass::init()
{
    VkDevice vkDevice          = m_context.getDevice().getHandle();
    VkFormat vkSwapchainFormat = m_swapchain.getFormat();

    VkResult result = VK_SUCCESS;

    // Color Attachment
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format         = vkSwapchainFormat;
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

    result = vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateRenderPass' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Render Pass.");
    }

    std::cout << "[INFO]\tRender Pass Created Successfully.\n";
}

void RenderPass::cleanup()
{
    VkDevice vkDevice = m_context.getDevice().getHandle();

    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(vkDevice, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}
