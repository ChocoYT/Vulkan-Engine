#include "Renderer/Renderer.hpp"

#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Resources/MemoryAllocator.hpp"
#include "Vulkan/Swapchain/Swapchain.hpp"
#include "Vulkan/RenderPass/RenderPass.hpp"
#include "Vulkan/Synchronization/Synchronization.hpp"
#include "Vulkan/Commands/CommandPool.hpp"
#include "Vulkan/Descriptors/DescriptorPool.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Mesh.hpp"

#include "Vulkan/Buffers/Uniform.hpp"

Renderer::Renderer(
    const Window &window,
    std::unique_ptr<Scene> scene
) : m_window(window), m_scene(std::move(scene)) {}

Renderer::~Renderer()
{
    cleanup();
}

void Renderer::init()
{
    // Context
    m_context = std::make_unique<Context>(m_window);
    m_context->init(FRAMES_IN_FLIGHT);

    // Memory Allocator
    m_allocator = std::make_unique<MemoryAllocator>(*m_context);

    // Swapchain
    m_swapchain = std::make_unique<Swapchain>(m_window, *m_context, *m_allocator);
    m_swapchain->init(FRAMES_IN_FLIGHT);

    // Render Pass
    m_renderPass = std::make_unique<RenderPass>(*m_context, *m_swapchain);
    m_renderPass->init();

    // Create Image Views and Framebuffers
    m_swapchain->createImageViews();
    m_swapchain->createFramebuffers(*m_renderPass);

    // Synchronization
    m_synchronization = std::make_unique<Synchronization>(*m_context);
    m_synchronization->init(FRAMES_IN_FLIGHT);

    // Command Pool and Buffers
    m_commandPool = std::make_unique<CommandPool>(*m_context);
    m_commandPool->init();
    m_commandPool->createCommandBuffers(FRAMES_IN_FLIGHT);
    
    // Descriptor Pool
    m_descriptorPool = std::make_unique<DescriptorPool>(*m_context);
    m_descriptorPool->init(FRAMES_IN_FLIGHT);

    // Initialize Scene Objects
    m_scene->getCamera()->initBuffer(*m_context, *m_commandPool, *m_descriptorPool, *m_allocator);
    
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
    layoutDescs.push_back(m_scene->getCamera()->getBuffer()->getDescriptorSetLayout());

    // Pipeline
    m_pipeline = std::make_unique<Pipeline>(*m_context, *m_swapchain, *m_renderPass, *m_synchronization);
    m_pipeline->init(bindingDescs, attrDescs, layoutDescs, FRAMES_IN_FLIGHT);
}

void Renderer::cleanup()
{
    if (m_context)
        vkDeviceWaitIdle(m_context->getDevice().getHandle());

    m_scene->cleanup();

    m_pipeline->cleanup();
    m_descriptorPool->cleanup();
    m_commandPool->cleanup();
    m_synchronization->cleanup();
    m_renderPass->cleanup();
    m_swapchain->cleanup();
    m_context->cleanup();
}

void Renderer::update(double deltaTime)
{
    m_scene->update(deltaTime);
}

void Renderer::draw()
{
    m_synchronization->waitForFence(m_currentFrame);

    VkCommandBuffer vkCommandBuffer = m_commandPool->beginFrame(m_currentFrame);
    uint32_t        imageIndex    = m_pipeline->beginFrame(vkCommandBuffer, m_currentFrame);
    
    m_pipeline->bind(vkCommandBuffer);

    // Draw Mesh
    for (auto &mesh : m_scene->getMeshes())
    {
        mesh->bind(vkCommandBuffer, m_currentFrame);
        mesh->draw(vkCommandBuffer);
    }

    m_pipeline->endFrame(vkCommandBuffer, m_currentFrame, imageIndex);

    m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
}
