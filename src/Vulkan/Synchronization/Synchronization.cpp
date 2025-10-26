#include "Vulkan/Synchronization/Synchronization.hpp"

#include "Vulkan/Synchronization/Semaphore.hpp"
#include "Vulkan/Synchronization/Fence.hpp"

Synchronization::Synchronization(const Context &context) : m_context(context) {}
Synchronization::~Synchronization()
{
    cleanup();
}

void Synchronization::init(uint32_t frameCount)
{
    // Create Semaphores
    for (uint32_t i = 0; i < frameCount; ++i)
    {
        m_imageSemaphores.emplace_back(std::make_unique<Semaphore>(m_context));
        m_imageSemaphores.back()->init(VK_SEMAPHORE_TYPE_BINARY);

        m_renderSemaphores.emplace_back(std::make_unique<Semaphore>(m_context));
        m_renderSemaphores.back()->init(VK_SEMAPHORE_TYPE_BINARY);
    }

    // Create Fences
    for (uint32_t i = 0; i < frameCount; ++i)
    {
        m_inFlightFences.emplace_back(std::make_unique<Fence>(m_context));
        m_inFlightFences.back()->init(VK_FENCE_CREATE_SIGNALED_BIT);
    }
}

void Synchronization::cleanup()
{
    // Destroy Semaphores
    for (auto &semaphore : m_imageSemaphores)
        semaphore->cleanup();
    for (auto &semaphore : m_renderSemaphores)
        semaphore->cleanup();
    
    m_imageSemaphores.clear();
    m_renderSemaphores.clear();

    // Destroy Fences
    for (auto &fence : m_inFlightFences)
        fence->cleanup();
    
    m_inFlightFences.clear();
}

void Synchronization::waitForFence(uint32_t currentFrame)
{
    m_inFlightFences[currentFrame]->wait();
}
