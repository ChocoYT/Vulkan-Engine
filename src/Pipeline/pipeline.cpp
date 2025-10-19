#include "Pipeline/pipeline.hpp"

#include "Core/context.hpp"
#include "Core/device.hpp"
#include "Core/swapchain.hpp"
#include "Core/render_pass.hpp"

void Pipeline::init(
    Context &context,
    std::vector<VkVertexInputBindingDescription>   &bindingDescs,
    std::vector<VkVertexInputAttributeDescription> &attrDescs,
    std::vector<VkDescriptorSetLayout>             &layoutDescs,
    uint32_t frameCount
) {
    m_context = &context;

    this->bindingDescs = bindingDescs;
    this->attrDescs    = attrDescs;
    this->layoutDescs  = layoutDescs;

    vertexShaderModule = createShaderModule("assets/shaders/renderVS.spv");
    pixelShaderModule  = createShaderModule("assets/shaders/renderPS.spv");

    createGraphicsPipeline();
    createSemaphores(frameCount);
    createFences(frameCount);
}

void Pipeline::cleanup()
{   
    VkDevice deviceHandle = m_context->getDevice().getHandle();

    // Destroy Shader Modules
    if (vertexShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(deviceHandle, vertexShaderModule, nullptr);
    if (pixelShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(deviceHandle, pixelShaderModule, nullptr);

    // Destroy Semaphores
    for (auto semaphore : imageAvailableSemaphores)
        vkDestroySemaphore(deviceHandle, semaphore, nullptr);
    for (auto semaphore : renderFinishedSemaphores)
        vkDestroySemaphore(deviceHandle, semaphore, nullptr);

    // Destroy Fences
    for (auto fence : inFlightFences)
        vkDestroyFence(deviceHandle, fence, nullptr);

    // Destroy Pipeline Layout
    if (graphicsPipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(deviceHandle, graphicsPipelineLayout, nullptr);
    
    // Destroy Graphics Pipeline
    if (graphicsPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(deviceHandle, graphicsPipeline, nullptr);
}

void Pipeline::bind(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

uint32_t Pipeline::beginFrame(VkCommandBuffer commandBuffer, int currentFrame)
{
    VkDevice       deviceHandle     = m_context->getDevice().getHandle();
    VkSwapchainKHR swapchainHandle  = m_context->getSwapchain().getHandle();
    VkRenderPass   renderPassHandle = m_context->getRenderPass().getHandle();

    VkResult result = VK_SUCCESS;

    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    uint32_t imageIndex;

    result = vkAcquireNextImageKHR(deviceHandle, swapchainHandle, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        std::cerr << "[ERROR]\t'vkAcquireNextImageKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Acquire Swapchain Image.");
    }

    // Begin Render Pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = renderPassHandle;
    renderPassInfo.framebuffer       = m_context->getSwapchain().getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_context->getSwapchain().getExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return imageIndex;
}

void Pipeline::endFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, int currentFrame)
{
    VkSwapchainKHR swapchainHandle = m_context->getSwapchain().getHandle();

    VkResult result = VK_SUCCESS;

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkEndCommandBuffer' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to End Command Buffer.");
    }

    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];

    // Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[]      = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount    = 1;
    submitInfo.pWaitSemaphores       = waitSemaphores;
    submitInfo.pWaitDstStageMask     = waitStages;
    submitInfo.commandBufferCount    = 1;
    submitInfo.pCommandBuffers       = &commandBuffer;

    VkSemaphore signalSemaphores[]  = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    result = vkQueueSubmit(m_context->getDevice().getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkQueueSubmit' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Submit Draw Command Buffer.");
    }

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapchains[] = { swapchainHandle };
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapchains;
    presentInfo.pImageIndices   = &imageIndex;

    vkQueuePresentKHR(m_context->getDevice().getPresentQueue(), &presentInfo);
}

void Pipeline::createGraphicsPipeline()
{
    VkDevice       deviceHandle     = m_context->getDevice().getHandle();
    VkRenderPass   renderPassHandle = m_context->getRenderPass().getHandle();

    VkResult result = VK_SUCCESS;

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
    VkExtent2D swapchainExtent = m_context->getSwapchain().getExtent();

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
            VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule, "main", nullptr 
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_FRAGMENT_BIT, pixelShaderModule, "main", nullptr
        }
    };

    // Pipeline Layout
    VkPipelineLayoutCreateInfo graphicsPipelineLayoutInfo{};
    graphicsPipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    graphicsPipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(layoutDescs.size());
    graphicsPipelineLayoutInfo.pSetLayouts            = layoutDescs.data();

    result = vkCreatePipelineLayout(deviceHandle, &graphicsPipelineLayoutInfo, nullptr, &graphicsPipelineLayout);
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
    pipelineInfo.layout              = graphicsPipelineLayout;
    pipelineInfo.renderPass          = renderPassHandle;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(deviceHandle, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateGraphicsPipelines' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Graphics Pipeline.");
    }

    std::cout << "[INFO]\tGraphics Pipeline Created Successfully.\n";
}

VkShaderModule Pipeline::createShaderModule(const std::string &filePath)
{
    VkDevice deviceHandle = m_context->getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    std::vector<char> code = readFileBinary(filePath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule = VK_NULL_HANDLE;

    result = vkCreateShaderModule(deviceHandle, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateShaderModule' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Shader Module.");
    }

    std::cout << "[INFO]\tShader Module Created Successfully.\n";

    return shaderModule;
}

VkSemaphore Pipeline::createSemaphore(VkSemaphoreType type)
{
    VkDevice deviceHandle = m_context->getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    VkSemaphoreTypeCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.semaphoreType = type;
    semaphoreCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &semaphoreCreateInfo;
    createInfo.flags = 0;

    VkSemaphore semaphore = VK_NULL_HANDLE;

    result = vkCreateSemaphore(deviceHandle, &createInfo, nullptr, &semaphore);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSemaphore' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Semaphore.");
    }

    std::cout << "[INFO]\tSemaphore Created Successfully.\n";

    return semaphore;
}

void Pipeline::createSemaphores(uint32_t frameCount)
{
    imageAvailableSemaphores.resize(frameCount);
    renderFinishedSemaphores.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        imageAvailableSemaphores[i] = createSemaphore(VK_SEMAPHORE_TYPE_BINARY);
        renderFinishedSemaphores[i] = createSemaphore(VK_SEMAPHORE_TYPE_BINARY);
    }
}

VkFence Pipeline::createFence(VkFenceCreateFlags flags)
{
    VkDevice deviceHandle = m_context->getDevice().getHandle();

    VkResult result = VK_SUCCESS;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;

    VkFence fence = VK_NULL_HANDLE;

    result = vkCreateFence(deviceHandle, &fenceInfo, nullptr, &fence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFence' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Fence.");
    }

    std::cout << "[INFO]\tFence Created Successfully.\n";

    return fence;
}

void Pipeline::createFences(uint32_t frameCount)
{
    VkResult result = VK_SUCCESS;

    inFlightFences.resize(frameCount);

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        inFlightFences[i] = createFence(VK_FENCE_CREATE_SIGNALED_BIT);
    }
}

void Pipeline::waitForFence(int currentFrame)
{
    VkDevice deviceHandle = m_context->getDevice().getHandle();

    VkFence fence = getInFlightFences()[currentFrame];
    vkWaitForFences(deviceHandle, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(deviceHandle, 1, &fence);
}
