#include "Vulkan/Pipeline/Pipeline.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Swapchain/Swapchain.hpp"
#include "Vulkan/Swapchain/Framebuffer.hpp"

#include "Vulkan/RenderPass/RenderPass.hpp"

#include "Vulkan/Pipeline/ShaderModule.hpp"

#include "Vulkan/Synchronization/Synchronization.hpp"
#include "Vulkan/Synchronization/Semaphore.hpp"
#include "Vulkan/Synchronization/Fence.hpp"

Pipeline::Pipeline(
    const Context         &context,
    const Swapchain       &swapchain,
    const RenderPass      &renderPass,
    const Synchronization &synchronization
) : m_context(context), m_swapchain(swapchain), m_renderPass(renderPass), m_synchronization(synchronization) {}

Pipeline::~Pipeline()
{
    cleanup();
}

void Pipeline::init(
    std::vector<VkVertexInputBindingDescription>   &bindingDescs,
    std::vector<VkVertexInputAttributeDescription> &attrDescs,
    std::vector<VkDescriptorSetLayout>             &layoutDescs,
    uint32_t frameCount
) {
    VkDevice deviceHandle = m_context.getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    m_bindingDescs = bindingDescs;
    m_attrDescs    = attrDescs;
    m_layoutDescs  = layoutDescs;

    m_vertexShaderModule = std::make_unique<ShaderModule>(m_context);
    m_vertexShaderModule->init("assets/shaders/renderVS.spv");
    m_pixelShaderModule  = std::make_unique<ShaderModule>(m_context);
    m_pixelShaderModule->init("assets/shaders/renderPS.spv");

    // Vertex Input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(m_bindingDescs.size());
    vertexInputInfo.pVertexBindingDescriptions      = m_bindingDescs.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attrDescs.size());
    vertexInputInfo.pVertexAttributeDescriptions    = m_attrDescs.data();

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkExtent2D swapchainExtent = m_swapchain.getExtent();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(swapchainExtent.width);
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
            VK_SHADER_STAGE_VERTEX_BIT, m_vertexShaderModule->getHandle(), "main", nullptr 
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_FRAGMENT_BIT, m_pixelShaderModule->getHandle(), "main", nullptr
        }
    };

    // Pipeline Layout
    VkPipelineLayoutCreateInfo graphicsPipelineLayoutInfo{};
    graphicsPipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    graphicsPipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(m_layoutDescs.size());
    graphicsPipelineLayoutInfo.pSetLayouts            = m_layoutDescs.data();

    result = vkCreatePipelineLayout(deviceHandle, &graphicsPipelineLayoutInfo, nullptr, &m_layout);
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
    pipelineInfo.layout              = m_layout;
    pipelineInfo.renderPass          = m_renderPass.getHandle();
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(deviceHandle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_handle);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateGraphicsPipelines' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Graphics Pipeline.");
    }

    std::cout << "[INFO]\tGraphics Pipeline Created Successfully.\n";
}

void Pipeline::cleanup()
{   
    VkDevice deviceHandle = m_context.getDevice().getHandle();

    // Destroy Shader Modules
    m_vertexShaderModule->cleanup();
    m_pixelShaderModule->cleanup();

    // Destroy Pipeline Layout
    if (m_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(deviceHandle, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }
    
    // Destroy Graphics Pipeline
    if (m_handle != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(deviceHandle, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
}

void Pipeline::bind(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_handle);
}

uint32_t Pipeline::beginFrame(VkCommandBuffer commandBuffer, uint32_t currentFrame)
{
    VkDevice       vkDevice    = m_context.getDevice().getHandle();
    VkSwapchainKHR vkSwapchain = m_swapchain.getHandle();

    VkResult result = VK_SUCCESS;

    VkSemaphore imageAvailableSemaphore = m_synchronization.getImageSemaphores()[currentFrame]->getHandle();

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(vkDevice, vkSwapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        std::cerr << "[ERROR]\t'vkAcquireNextImageKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Acquire Swapchain Image.");
    }

    // Begin Render Pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = m_renderPass.getHandle();
    renderPassInfo.framebuffer       = m_swapchain.getFramebuffers()[imageIndex]->getHandle();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapchain.getExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return imageIndex;
}

void Pipeline::endFrame(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t imageIndex)
{
    VkSwapchainKHR swapchainHandle = m_swapchain.getHandle();

    VkResult result = VK_SUCCESS;

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEndCommandBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to End Command Buffer.");
    }

    VkSemaphore imageAvailableSemaphore = m_synchronization.getImageSemaphores()[currentFrame]->getHandle();
    VkSemaphore renderFinishedSemaphore = m_synchronization.getRenderSemaphores()[currentFrame]->getHandle();

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
    submitInfo.pCommandBuffers      = &commandBuffer;

    result = vkQueueSubmit(m_context.getDevice().getGraphicsQueue(), 1, &submitInfo, m_synchronization.getInFlightFences()[currentFrame]->getHandle());
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

    vkQueuePresentKHR(m_context.getDevice().getPresentQueue(), &presentInfo);
}
