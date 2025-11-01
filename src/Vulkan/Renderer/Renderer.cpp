#include "Vulkan/Renderer/Renderer.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Resources/MemoryAllocator.hpp"
#include "Vulkan/Swapchain/Swapchain.hpp"
#include "Vulkan/RenderPass/RenderPass.hpp"
#include "Vulkan/Sync/Sync.hpp"
#include "Vulkan/Commands/CommandPool.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Mesh.hpp"

#include "Vulkan/Buffers/Uniform.hpp"

VulkanRenderer::VulkanRenderer(
    std::unique_ptr<VulkanContext>         context,
    std::unique_ptr<VulkanMemoryAllocator> allocator,
    std::unique_ptr<VulkanSwapchain>       swapchain,
    std::unique_ptr<VulkanRenderPass>      renderPass,
    std::unique_ptr<VulkanSync>            sync,
    std::unique_ptr<VulkanCommandPool>     commandPool,
    std::unique_ptr<VulkanDescriptorPool>  descriptorPool
) : m_context       (std::move(context)),
    m_allocator     (std::move(allocator)),
    m_swapchain     (std::move(swapchain)),
    m_renderPass    (std::move(renderPass)),
    m_sync          (std::move(sync)),
    m_commandPool   (std::move(commandPool)),
    m_descriptorPool(std::move(descriptorPool))
{}

VulkanRenderer::~VulkanRenderer() = default;

std::unique_ptr<VulkanRenderer> VulkanRenderer::Create(const Window &window)
{
    // Context
    auto context = VulkanContext::Create(window, FRAMES_IN_FLIGHT);

    // Memory Allocator
    auto allocator = VulkanMemoryAllocator::Create();

    // Swapchain
    auto swapchain = VulkanSwapchain::Create(
        window,
        context->GetSurface(),
        context->GetPhysicalDevice(),
        context->GetDevice(),
        FRAMES_IN_FLIGHT
    );

    // Render Pass
    auto renderPass = VulkanRenderPass::Create(
        context->GetDevice(),
        *swapchain
    );

    // Create Swapchain Resources
    swapchain->CreateImages(
        context->GetPhysicalDevice(),
        *allocator
    );
    swapchain->CreateImageViews();
    swapchain->CreateFramebuffers(*renderPass);

    // Synchronization
    auto sync = VulkanSync::Create(
        context->GetDevice(),
        FRAMES_IN_FLIGHT
    );

    // Command Pool and Buffers
    auto commandPool = VulkanCommandPool::Create(context->GetDevice());
    commandPool->CreateCommandBuffers(FRAMES_IN_FLIGHT);
    
    // Descriptor Pool
    auto descriptorPool = VulkanDescriptorPool::Create(context->GetDevice(), FRAMES_IN_FLIGHT);

    return std::unique_ptr<VulkanRenderer>(new VulkanRenderer(
        std::move(context),
        std::move(allocator),
        std::move(swapchain),
        std::move(renderPass),
        std::move(sync),
        std::move(commandPool),
        std::move(descriptorPool)
    ));
}

void VulkanRenderer::Finish()
{
    if (m_context)
        vkDeviceWaitIdle(m_context->GetDevice().GetHandle());
}

void VulkanRenderer::Draw()
{
    m_sync->WaitForFence(m_currentFrame);

    // Begin Frame
    VkCommandBuffer vkCommandBuffer = m_commandPool->BeginFrame(m_currentFrame);
    uint32_t        imageIndex      = m_pipeline->BeginFrame(
        *m_swapchain,
        *m_renderPass,
        *m_sync,
        vkCommandBuffer,
        m_currentFrame
    );
    
    m_pipeline->Bind(vkCommandBuffer, m_scene->GetDescriptorSets(m_currentFrame));

    // Bind Meshes
    for (auto &mesh : m_scene->GetMeshes())
    {
        mesh->Bind(vkCommandBuffer, m_currentFrame);
    }

    // Draw Meshes
    for (auto &mesh : m_scene->GetMeshes())
    {
        mesh->Draw(vkCommandBuffer);
    }

    // End Frame
    m_pipeline->EndFrame(
        *m_swapchain,
        *m_sync,
        vkCommandBuffer,
        m_currentFrame,
        imageIndex
    );

    m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
}

void VulkanRenderer::SetScene(std::shared_ptr<VulkanScene> scene)
{
    m_scene = std::move(scene);

    // Binding Description
    std::vector<VkVertexInputBindingDescription> bindingDescs(1);
    bindingDescs[0].binding   = 0;  // Vertex
    bindingDescs[0].stride    = sizeof(Vertex);
    bindingDescs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute Description
    std::vector<VkVertexInputAttributeDescription> attrDescs(2);
    attrDescs[0].binding  = 0;  // Vertex Position
    attrDescs[0].location = 0;
    attrDescs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[0].offset   = offsetof(Vertex, pos);

    attrDescs[1].binding  = 0;  // Vertex Color
    attrDescs[1].location = 1;
    attrDescs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[1].offset   = offsetof(Vertex, color);

    // Layout Description
    std::vector<VkDescriptorSetLayout> layoutDescs;
    layoutDescs.insert(
        layoutDescs.end(),
        m_scene->GetDescriptorSetLayouts().begin(),
        m_scene->GetDescriptorSetLayouts().end()
    );
    
    // Pipeline
    auto pipeline = VulkanPipeline::Create(
        m_context->GetDevice(),
        *m_swapchain,
        *m_renderPass,
        bindingDescs,
        attrDescs,
        layoutDescs,
        FRAMES_IN_FLIGHT
    );

    m_pipeline = std::move(pipeline);
}

VulkanRenderer::VulkanRenderer(VulkanRenderer&& other) noexcept : 
    m_context(std::move(other.m_context)),
    m_allocator(std::move(other.m_allocator)),
    m_swapchain(std::move(other.m_swapchain)),
    m_renderPass(std::move(other.m_renderPass)),
    m_sync(std::move(other.m_sync)),
    m_commandPool(std::move(other.m_commandPool)),
    m_descriptorPool(std::move(other.m_descriptorPool)),
    m_pipeline(std::move(other.m_pipeline)),
    m_scene(std::move(other.m_scene)),
    m_currentFrame(other.m_currentFrame)
{
    other = VulkanRenderer{};
}

VulkanRenderer& VulkanRenderer::operator=(VulkanRenderer &&other) noexcept
{
    if (this != &other)
    {
        m_context        = std::move(other.m_context);
        m_allocator      = std::move(other.m_allocator);
        m_swapchain      = std::move(other.m_swapchain);
        m_renderPass     = std::move(other.m_renderPass);
        m_sync           = std::move(other.m_sync);
        m_commandPool    = std::move(other.m_commandPool);
        m_descriptorPool = std::move(other.m_descriptorPool);
        m_pipeline       = std::move(other.m_pipeline);
        m_scene          = std::move(other.m_scene);
        m_currentFrame   = other.m_currentFrame;

        other = VulkanRenderer{};
    }

    return *this;
}
