#include "vulkan_pipeline.hpp"

void VulkanPipeline::init(VulkanContext &vulkanContext, std::vector<VkVertexInputBindingDescription> &bindingDescs, std::vector<VkVertexInputAttributeDescription> &attrDescs)
{
    context = &vulkanContext;

    this->bindingDescs = &bindingDescs;
    this->attrDescs    = &attrDescs;

    vertexShaderModule = createShaderModule("shaders/renderVS.spv");
    pixelShaderModule  = createShaderModule("shaders/renderPS.spv");

    createGraphicsPipeline();
    createSemaphores();
    createFences();
}

void VulkanPipeline::cleanup()
{   
    // Destroy Shader Modules
    if (vertexShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(context->getDevice(), vertexShaderModule, nullptr);
    if (pixelShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(context->getDevice(), pixelShaderModule, nullptr);

    // Destroy Semaphores
    for (auto semaphore : imageAvailableSemaphores)
        vkDestroySemaphore(context->getDevice(), semaphore, nullptr);
    for (auto semaphore : renderFinishedSemaphores)
        vkDestroySemaphore(context->getDevice(), semaphore, nullptr);

    // Destroy Fences
    for (auto fence : inFlightFences)
        vkDestroyFence(context->getDevice(), fence, nullptr);

    // Destroy Pipeline Layout
    if (graphicsPipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(context->getDevice(), graphicsPipelineLayout, nullptr);
    
    // Destroy Graphics Pipeline
    if (graphicsPipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(context->getDevice(), graphicsPipeline, nullptr);
}

void VulkanPipeline::bind(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}

uint32_t VulkanPipeline::beginFrame(VkCommandBuffer commandBuffer, int currentFrame)
{
    VkResult result = VK_SUCCESS;

    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    uint32_t imageIndex;

    result = vkAcquireNextImageKHR(context->getDevice(), context->getSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        std::cerr << "[ERROR]\t'vkAcquireNextImageKHR' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Acquire Swapchain Image.");
    }

    // Begin Render Pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass        = context->getRenderPass();
    renderPassInfo.framebuffer       = context->getSwapchainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = context->getSwapchainExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return imageIndex;
}

void VulkanPipeline::endFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, int currentFrame)
{
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

    result = vkQueueSubmit(context->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
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

    VkSwapchainKHR swapchains[] = { context->getSwapchain() };
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapchains;
    presentInfo.pImageIndices   = &imageIndex;

    vkQueuePresentKHR(context->getPresentQueue(), &presentInfo);
}

void VulkanPipeline::createGraphicsPipeline()
{
    VkResult result = VK_SUCCESS;

    // Vertex Input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(bindingDescs->size());
    vertexInputInfo.pVertexBindingDescriptions      = bindingDescs->data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs->size());
    vertexInputInfo.pVertexAttributeDescriptions    = attrDescs->data();

    // Input Assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(context->getSwapchainExtent().width);
    viewport.height = static_cast<float>(context->getSwapchainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = context->getSwapchainExtent();

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
    graphicsPipelineLayoutInfo.setLayoutCount         = 0;
    graphicsPipelineLayoutInfo.pushConstantRangeCount = 0;

    result = vkCreatePipelineLayout(context->getDevice(), &graphicsPipelineLayoutInfo, nullptr, &graphicsPipelineLayout);
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
    pipelineInfo.renderPass          = context->getRenderPass();
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(context->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateGraphicsPipelines' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Graphics Pipeline.");
    }

    std::cout << "[INFO]\tGraphics Pipeline Created Successfully.\n";
}

VkShaderModule VulkanPipeline::createShaderModule(const std::string &filePath)
{
    VkResult result = VK_SUCCESS;

    std::vector<char> code = readFileBinary(filePath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule = VK_NULL_HANDLE;

    result = vkCreateShaderModule(context->getDevice(), &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateShaderModule' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Shader Module.");
    }

    std::cout << "[INFO]\tShader Module Created Successfully.\n";

    return shaderModule;
}

VkSemaphore VulkanPipeline::createSemaphore(VkSemaphoreType type)
{
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

    result = vkCreateSemaphore(context->getDevice(), &createInfo, nullptr, &semaphore);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateSemaphore' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Semaphore.");
    }

    std::cout << "[INFO]\tSemaphore Created Successfully.\n";

    return semaphore;
}

void VulkanPipeline::createSemaphores()
{
    imageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);

    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        imageAvailableSemaphores[i] = createSemaphore(VK_SEMAPHORE_TYPE_BINARY);
        renderFinishedSemaphores[i] = createSemaphore(VK_SEMAPHORE_TYPE_BINARY);
    }
}

VkFence VulkanPipeline::createFence(VkFenceCreateFlags flags)
{
    VkResult result = VK_SUCCESS;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = flags;

    VkFence fence = VK_NULL_HANDLE;

    result = vkCreateFence(context->getDevice(), &fenceInfo, nullptr, &fence);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateFence' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Fence.");
    }

    std::cout << "[INFO]\tFence Created Successfully.\n";

    return fence;
}

void VulkanPipeline::createFences()
{
    VkResult result = VK_SUCCESS;

    inFlightFences.resize(FRAMES_IN_FLIGHT);

    for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        inFlightFences[i] = createFence(VK_FENCE_CREATE_SIGNALED_BIT);
    }
}

void VulkanPipeline::waitForFence(int currentFrame)
{
    VkFence fence = getInFlightFences()[currentFrame];
    vkWaitForFences(context->getDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(context->getDevice(), 1, &fence);
}