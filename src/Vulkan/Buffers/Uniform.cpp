#include "Vulkan/Buffers/Uniform.hpp"

#include <iostream>
#include <stdexcept>

#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"

#include "Vulkan/Resources/Buffer.hpp"

VulkanUniformBuffer::VulkanUniformBuffer(
    const VulkanDevice &device,
    std::vector<std::unique_ptr<VulkanBuffer>> buffers,
    std::vector<VkDescriptorSet> descriptorSets,
    VkDescriptorSetLayout        descriptorSetLayout,
    VkDeviceSize size,
    uint32_t     count
) : m_device(device),
    m_buffers(std::move(buffers)),
    m_descriptorSets(std::move(descriptorSets)),
    m_descriptorSetLayout(descriptorSetLayout),
    m_size(size),
    m_count(count)
{}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
    Cleanup();
}

std::unique_ptr<VulkanUniformBuffer> VulkanUniformBuffer::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    const VulkanDescriptorPool &descriptorPool,
    VulkanMemoryAllocator      &allocator,
    VkDeviceSize size,
    uint32_t     count
) {
    VkResult result = VK_SUCCESS;

    // Initialize Uniform Buffers
    std::vector<std::unique_ptr<VulkanBuffer>> buffers;

    for (uint32_t i = 0; i < count; ++i)
    {
        buffers.emplace_back(VulkanBuffer::Create(
            physicalDevice,
            device,
            allocator,
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ));
    }

    // Create Descriptor Set Layout
    VkDescriptorSetLayoutBinding bufferLayout{};
    bufferLayout.binding         = 0;
    bufferLayout.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bufferLayout.descriptorCount = 1;
    bufferLayout.stageFlags      = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings    = &bufferLayout;

    // Create Descriptor Layout
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    result = vkCreateDescriptorSetLayout(device.GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkCreateDescriptorSetLayout' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Create Descriptor Set Layout.");
    }

    // Allocate Descriptor Sets
    std::vector<VkDescriptorSet>       descriptorSets(count);
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts(count, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool.GetHandle();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts        = descriptorSetLayouts.data();

    result = vkAllocateDescriptorSets(device.GetHandle(), &allocInfo, descriptorSets.data()); 
    if (result != VK_SUCCESS)
    {
        std::cerr << "[ERROR]\t'vkAllocateDescriptorSets' Failed with Error Code " << result << "\n";

        throw std::runtime_error("Failed to Allocate Descriptor Sets.");
    }

    // Update Uniform Buffers
    for (size_t i = 0; i < buffers.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers[i]->GetHandle();
        bufferInfo.offset = 0;
        bufferInfo.range  = size; 

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = descriptorSets[i];
        write.dstBinding      = 0;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo     = &bufferInfo;

        vkUpdateDescriptorSets(device.GetHandle(), 1, &write, 0, nullptr); 
    }

    return std::unique_ptr<VulkanUniformBuffer>(
        new VulkanUniformBuffer(
            device,
            std::move(buffers),
            std::move(descriptorSets),
            descriptorSetLayout,
            size,
            count
        )
    );
}

void VulkanUniformBuffer::Cleanup()
{
    m_buffers.clear();
    
    if (m_descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(m_device.GetHandle(), m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }
}

void VulkanUniformBuffer::Bind(
    VulkanPipeline  &pipeline,
    VkCommandBuffer commandBuffer,
    uint32_t        currentFrame
) {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline.GetLayout(),
        0, 1,
        &m_descriptorSets[currentFrame],
        0, nullptr
    );
}

void VulkanUniformBuffer::Update(
    void     *data,
    uint32_t currentFrame
) {
    m_buffers[currentFrame]->Update(data, m_size);
}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBuffer&& other) noexcept : 
    m_device(other.m_device),
    m_buffers(std::move(other.m_buffers)),
    m_descriptorSets(std::move(other.m_descriptorSets)),
    m_descriptorSetLayout(other.m_descriptorSetLayout),
    m_size(other.m_size),
    m_count(other.m_count)
{
    other.m_descriptorSetLayout = VK_NULL_HANDLE;
    other.m_size  = 0; 
    other.m_count = 0;
}

VulkanUniformBuffer& VulkanUniformBuffer::operator=(VulkanUniformBuffer &&other) noexcept
{
    if (this != &other)
    {
        Cleanup(); 

        m_buffers             = std::move(other.m_buffers);
        m_descriptorSets      = std::move(other.m_descriptorSets);
        m_descriptorSetLayout = other.m_descriptorSetLayout;
        m_size                = other.m_size;
        m_count               = other.m_count;

        other.m_descriptorSetLayout = VK_NULL_HANDLE;
        other.m_size  = 0;
        other.m_count = 0;
    }
    return *this;
}
