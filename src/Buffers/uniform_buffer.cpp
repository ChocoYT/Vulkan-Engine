#include "Buffers/uniform_buffer.hpp"

void UniformBuffer::init(VulkanContext &context, VkDeviceSize bufferSize)
{
    VkResult result = VK_SUCCESS;

    this->bufferSize = bufferSize;

    uniformBuffers = VulkanBuffer::createUniformBuffers(context, nullptr, bufferSize);

    // Create Descriptor Set Layout
    VkDescriptorSetLayoutBinding uniformBufferLayout{};
    uniformBufferLayout.binding = 0;
    uniformBufferLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayout.descriptorCount = 1;
    uniformBufferLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uniformBufferLayout;

    result = vkCreateDescriptorSetLayout(context.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorSetLayout' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Set Layout.");
    }

    descriptorSets.resize(uniformBuffers.size());
    std::vector<VkDescriptorSetLayout> layouts(uniformBuffers.size(), descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = context.getDescriptorPool();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    result = vkAllocateDescriptorSets(context.getDevice(), &allocInfo, descriptorSets.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateDescriptorSets' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Descriptor Sets.");
    }

    // Write to Buffers
    for (size_t i = 0; i < uniformBuffers.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSets[i];
        write.dstBinding = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(context.getDevice(), 1, &write, 0, nullptr);
    }
}

void UniformBuffer::cleanup(VulkanContext &context)
{
    for (auto &buffer : uniformBuffers)
        buffer.cleanup();

    vkDestroyDescriptorSetLayout(context.getDevice(), descriptorSetLayout, nullptr);
}

void UniformBuffer::update(int currentFrame, void *bufferData)
{
    uniformBuffers[currentFrame].writeData(bufferData, bufferSize);
}

void UniformBuffer::bind(VulkanContext &vulkanContext, VulkanPipeline &vulkanPipeline, int currentFrame)
{
    vkCmdBindDescriptorSets(
        vulkanContext.getCommandBuffers()[currentFrame],
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        vulkanPipeline.getPipelineLayout(),
        0, 1,
        &descriptorSets[currentFrame],
        0, nullptr
    );
}
