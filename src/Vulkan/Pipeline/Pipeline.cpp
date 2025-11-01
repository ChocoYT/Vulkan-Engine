#include "Vulkan/Pipeline/Pipeline.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Swapchain/Swapchain.hpp"
#include "Vulkan/Swapchain/Framebuffer.hpp"
#include "Vulkan/RenderPass/RenderPass.hpp"

#include "Vulkan/Pipeline/ShaderModule.hpp"

#include "Vulkan/Sync/Sync.hpp"
#include "Vulkan/Sync/Semaphore.hpp"
#include "Vulkan/Sync/Fence.hpp"

VulkanPipeline::VulkanPipeline(
    const VulkanDevice &device,
    VkPipeline       handle,
    VkPipelineLayout layout,
    std::unique_ptr<VulkanShaderModule> vertexShaderModule,
    std::unique_ptr<VulkanShaderModule> pixelShaderModule
) : m_device(device),
    m_handle(handle),
    m_layout(layout),
    m_vertexShaderModule(std::move(vertexShaderModule)),
    m_pixelShaderModule(std::move(pixelShaderModule))
{}

VulkanPipeline::~VulkanPipeline()
{
    Cleanup();
}

std::unique_ptr<VulkanPipeline> VulkanPipeline::Create(
    const VulkanDevice     &device,
    const VulkanSwapchain  &swapchain,
    const VulkanRenderPass &renderPass,
    std::vector<VkVertexInputBindingDescription>   &bindingDescs,
    std::vector<VkVertexInputAttributeDescription> &attrDescs,
    std::vector<VkDescriptorSetLayout>             &layoutDescs,
    uint32_t frameCount
) {
    VkResult result = VK_SUCCESS;

    auto vertexShaderModule = VulkanShaderModule::Create(device, "assets/shaders/renderVS.spv");
    auto pixelShaderModule  = VulkanShaderModule::Create(device, "assets/shaders/renderPS.spv");

    // Vertex Input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(bindingDescs.size());
    vertexInputInfo.pVertexBindingDescriptions      = bindingDescs.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
    vertexInputInfo.pVertexAttributeDescriptions    = attrDescs.data();

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkExtent2D swapchainExtent = swapchain.GetExtent();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(swapchain.GetExtent().width);
    viewport.height = static_cast<float>(swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Color Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    // Shader Stages
    VkPipelineShaderStageCreateInfo shaderStages[] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule->GetHandle(), "main", nullptr 
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_FRAGMENT_BIT, pixelShaderModule->GetHandle(), "main", nullptr
        }
    };

    VkPipelineLayoutCreateInfo graphicsPipelineLayoutInfo{};
    graphicsPipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    graphicsPipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(layoutDescs.size());
    graphicsPipelineLayoutInfo.pSetLayouts            = layoutDescs.data();

    // Create Pipeline Layout
    VkPipelineLayout layout = VK_NULL_HANDLE;
    result = vkCreatePipelineLayout(device.GetHandle(), &graphicsPipelineLayoutInfo, nullptr, &layout);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreatePipelineLayout' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to create Graphics Pipeline Layout.");
    }

    std::cout << "[INFO]\tGraphics Pipeline Layout Created Successfully.\n";

    // Graphics Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.layout              = layout;
    pipelineInfo.renderPass          = renderPass.GetHandle();
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    // Create Pipeline
    VkPipeline handle = VK_NULL_HANDLE;
    result = vkCreateGraphicsPipelines(device.GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateGraphicsPipelines' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Graphics Pipeline.");
    }

    std::cout << "[INFO]\tGraphics Pipeline Created Successfully.\n";

    return std::unique_ptr<VulkanPipeline>(
        new VulkanPipeline(
            device,
            handle,
            layout,
            std::move(vertexShaderModule),
            std::move(pixelShaderModule)
        )
    );
}

void VulkanPipeline::Cleanup()
{   
    // Destroy Graphics Pipeline
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device.GetHandle(), m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }

    // Destroy Pipeline Layout
    if (m_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device.GetHandle(), m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }
}

void VulkanPipeline::Bind(
    VkCommandBuffer vkCommandBuffer,
    const std::vector<VkDescriptorSet> &vkDescriptorSets
) {
    vkCmdBindPipeline(
        vkCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_handle
    );

    vkCmdBindDescriptorSets(
        vkCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_layout,
        0,
        static_cast<uint32_t>(vkDescriptorSets.size()),
        vkDescriptorSets.data(),
        0,
        nullptr
    );
}

uint32_t VulkanPipeline::BeginFrame(
    const VulkanSwapchain  &swapchain,
    const VulkanRenderPass &renderPass,
    const VulkanSync       &sync,
    VkCommandBuffer vkCommandBuffer,
    uint32_t        currentFrame
) {
    VkResult result = VK_SUCCESS;

    VkSemaphore imageAvailableSemaphore = sync.GetImageSemaphores()[currentFrame]->GetHandle();

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(m_device.GetHandle(), swapchain.GetHandle(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        std::cerr << "[ERROR]\t'vkAcquireNextImageKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Acquire Swapchain Image.");
    }

    // Begin Render Pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPass.GetHandle();
    renderPassInfo.framebuffer       = swapchain.GetFramebuffers()[imageIndex]->GetHandle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain.GetExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return imageIndex;
}

void VulkanPipeline::EndFrame(
    const VulkanSwapchain &swapchain,
    const VulkanSync      &sync,
    VkCommandBuffer vkCommandBuffer,
    uint32_t        currentFrame,
    uint32_t        imageIndex
) {
    VkSwapchainKHR swapchainHandle = swapchain.GetHandle();

    VkResult result = VK_SUCCESS;

    vkCmdEndRenderPass(vkCommandBuffer);

    result = vkEndCommandBuffer(vkCommandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEndCommandBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to End Command Buffer.");
    }

    VkSemaphore imageAvailableSemaphore = sync.GetImageSemaphores()[currentFrame]->GetHandle();
    VkSemaphore renderFinishedSemaphore = sync.GetRenderSemaphores()[currentFrame]->GetHandle();

    // Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkSemaphore> waitSemaphores   = { imageAvailableSemaphore };
    std::vector<VkSemaphore> signalSemaphores = { renderFinishedSemaphore };
    std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    
    submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores      = waitSemaphores.data();
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores    = signalSemaphores.data();
    submitInfo.pWaitDstStageMask    = waitStages.data();
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &vkCommandBuffer;

    result = vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, sync.GetInFlightFences()[currentFrame]->GetHandle());
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkQueueSubmit' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Submit Draw Command Buffer.");
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    presentInfo.pWaitSemaphores    = signalSemaphores.data();

    std::vector<VkSwapchainKHR> swapchains = { swapchainHandle };
    presentInfo.swapchainCount  = static_cast<uint32_t>(swapchains.size());
    presentInfo.pSwapchains     = swapchains.data();
    presentInfo.pImageIndices   = &imageIndex;

    vkQueuePresentKHR(m_device.GetPresentQueue(), &presentInfo);
}

VulkanPipeline::VulkanPipeline(VulkanPipeline &&other) noexcept : 
    m_device(other.m_device),
    m_handle(other.m_handle),
    m_layout(other.m_layout),
    m_vertexShaderModule(std::move(other.m_vertexShaderModule)),
    m_pixelShaderModule(std::move(other.m_pixelShaderModule))
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_layout = VK_NULL_HANDLE;
}

VulkanPipeline& VulkanPipeline::operator=(VulkanPipeline &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        m_handle = other.m_handle;
        m_layout = other.m_layout;
        m_vertexShaderModule = std::move(other.m_vertexShaderModule);
        m_pixelShaderModule  = std::move(other.m_pixelShaderModule);

        other.m_handle = VK_NULL_HANDLE;
        other.m_layout = VK_NULL_HANDLE;
    }

    return *this;
}
