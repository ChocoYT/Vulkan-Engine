#include "Vulkan/Buffers/Uniform.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"

#include "Vulkan/Descriptors/DescriptorPool.hpp"

#include "Vulkan/Resources/Buffer.hpp"
#include "Vulkan/Resources/MemoryAllocator.hpp"

UniformBuffer::UniformBuffer(
    const Context        &context,
    const CommandPool    &commandPool,
    const DescriptorPool &descriptorPool,
    MemoryAllocator      &allocator
) : m_context(context), m_commandPool(commandPool), m_descriptorPool(descriptorPool), m_allocator(allocator) {}

UniformBuffer::~UniformBuffer()
{
    cleanup();
}

void UniformBuffer::init(
    VkDeviceSize bufferSize,
    uint32_t     bufferCount
) {
    VkDevice         device         = m_context.getDevice().getHandle();
    VkDescriptorPool descriptorPool = m_descriptorPool.getHandle();

    m_bufferSize  = bufferSize;
    m_bufferCount = bufferCount;

    VkResult result = VK_SUCCESS;

    // Initialize Uniform Buffers
    for (uint32_t i = 0; i < bufferCount; ++i)
    {
        m_buffers.emplace_back(std::make_unique<Buffer>(m_context, m_commandPool, m_allocator));
        m_buffers.back()->init(
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
    }

    // Create Descriptor Set Layout
    VkDescriptorSetLayoutBinding uniformBufferLayout{};
    uniformBufferLayout.binding         = 0;
    uniformBufferLayout.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayout.descriptorCount = 1;
    uniformBufferLayout.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &uniformBufferLayout;

    result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_descriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorSetLayout' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Set Layout.");
    }

    // Allocate Descriptor Sets
    m_descriptorSets.resize(m_bufferCount); 
    std::vector<VkDescriptorSetLayout> layouts(m_bufferCount, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    result = vkAllocateDescriptorSets(device, &allocInfo, m_descriptorSets.data()); 
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateDescriptorSets' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Descriptor Sets.");
    }

    // Write to Uniform Buffers
    for (size_t i = 0; i < m_buffers.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_buffers[i]->getHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = m_bufferSize; 

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSets[i];
        write.dstBinding = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr); 
    }
}

void UniformBuffer::cleanup()
{
    VkDevice device = m_context.getDevice().getHandle();

    m_buffers.clear();
    
    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }
}

void UniformBuffer::bind(
    VkCommandBuffer  commandBuffer,
    VkPipelineLayout pipelineLayout,
    uint32_t         currentFrame
) {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0, 1,
        &m_descriptorSets[currentFrame],
        0, nullptr
    );
}

void UniformBuffer::update(
    void     *bufferData,
    uint32_t currentFrame
) {
    m_buffers[currentFrame]->update(bufferData, m_bufferSize);
}
