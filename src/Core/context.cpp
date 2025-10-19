#include "Core/context.hpp"

#include "Core/instance.hpp"
#include "Core/debug.hpp"
#include "Core/surface.hpp"
#include "Core/physical_device.hpp"
#include "Core/device.hpp"
#include "Core/swapchain.hpp"
#include "Core/render_pass.hpp"
#include "Core/command_pool.hpp"
#include "Core/descriptor_pool.hpp"

Context::Context()  = default;
Context::~Context()
{
    cleanup();
}

void Context::init(
    GLFWwindow *window,
    uint32_t   frameCount
) {
    // Instance
    m_instance = std::make_unique<Instance>();
    m_instance->init();

    // Debugger
    m_debug  = std::make_unique<Debug>();
    m_debug->init(m_instance->getHandle());

    // Surface
    m_surface = std::make_unique<Surface>();
    m_surface->init(
        m_instance->getHandle(),
        window
    );

    // Physical Device
    m_physicalDevice = std::make_unique<PhysicalDevice>();
    m_physicalDevice->init(
        m_instance->getHandle(),
        m_surface->getHandle()
    );

    // Device
    m_device = std::make_unique<Device>();
    m_device->init(
        m_physicalDevice->getHandle(),
        m_physicalDevice->getGraphicsQueueFamily(),
        m_physicalDevice->getPresentQueueFamily()
    );

    // Swapchain
    m_swapchain = std::make_unique<Swapchain>();
    m_swapchain->init(
        m_surface->getHandle(),
        m_physicalDevice->getHandle(),
        m_device->getHandle(),
        window,
        m_physicalDevice->getGraphicsQueueFamily(),
        m_physicalDevice->getPresentQueueFamily(),
        frameCount
    );

    // Render Pass
    m_renderPass = std::make_unique<RenderPass>();
    m_renderPass->init(
        m_device->getHandle(),
        m_swapchain->getHandle(),
        m_swapchain->getFormat()
    );

    // Create Image Views and Framebuffers
    m_swapchain->createImageViews(m_device->getHandle());
    m_swapchain->createFramebuffers(
        m_device->getHandle(),
        m_renderPass->getHandle()
    );

    // Command Pool and Buffers
    m_commandPool = std::make_unique<CommandPool>();
    m_commandPool->init(
        m_device->getHandle(),
        m_physicalDevice->getGraphicsQueueFamily()
    );
    m_commandPool->createCommandBuffers(m_device->getHandle(), frameCount);

    // Descriptor Pool
    m_descriptorPool = std::make_unique<DescriptorPool>();
    m_descriptorPool->init(
        m_device->getHandle(),
        static_cast<uint32_t>(m_swapchain->getImages().size())
    );
}

void Context::cleanup()
{
    m_descriptorPool->cleanup(m_device->getHandle());
    m_commandPool->cleanup(m_device->getHandle());

    m_swapchain->cleanup(m_device->getHandle());
    m_renderPass->cleanup(m_device->getHandle());
    m_device->cleanup();
    m_surface->cleanup(m_instance->getHandle());
    m_debug->cleanup(m_instance->getHandle());
    m_instance->cleanup();
}

VkCommandBuffer Context::beginFrame(int currentFrame)
{
    return m_commandPool->beginFrame(currentFrame);
}
