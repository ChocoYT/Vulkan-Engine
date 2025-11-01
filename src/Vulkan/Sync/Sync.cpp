#include "Vulkan/Sync/Sync.hpp"

#include <cstdint>

#include "Vulkan/Sync/Semaphore.hpp"
#include "Vulkan/Sync/Fence.hpp"

VulkanSync::VulkanSync(
    std::vector<std::unique_ptr<VulkanFence>>     inFlightFences,
    std::vector<std::unique_ptr<VulkanSemaphore>> imageSemaphores,
    std::vector<std::unique_ptr<VulkanSemaphore>> renderSemaphores
) : m_inFlightFences(std::move(inFlightFences)),
    m_imageSemaphores(std::move(imageSemaphores)),
    m_renderSemaphores(std::move(renderSemaphores))
{}

VulkanSync::~VulkanSync()
{
    Cleanup();
}

std::unique_ptr<VulkanSync> VulkanSync::Create(
    const VulkanDevice &device,
    uint32_t frameCount
) {
    // Create Fences
    std::vector<std::unique_ptr<VulkanFence>> inFlightFences;

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        inFlightFences.emplace_back(VulkanFence::Create(
            device,
            VK_FENCE_CREATE_SIGNALED_BIT
        ));
    }

    // Create Semaphores
    std::vector<std::unique_ptr<VulkanSemaphore>> imageSemaphores;
    std::vector<std::unique_ptr<VulkanSemaphore>> renderSemaphores;

    for (uint32_t i = 0; i < frameCount; ++i)
    {
        imageSemaphores.emplace_back(VulkanSemaphore::Create(
            device,
            VK_SEMAPHORE_TYPE_BINARY
        ));
        renderSemaphores.emplace_back(VulkanSemaphore::Create(
            device,
            VK_SEMAPHORE_TYPE_BINARY
        ));
    }

    return std::unique_ptr<VulkanSync>(
        new VulkanSync(
            std::move(inFlightFences),
            std::move(imageSemaphores),
            std::move(renderSemaphores)
        )
    );
}

void VulkanSync::Cleanup()
{
    m_imageSemaphores.clear();
    m_renderSemaphores.clear();
    m_inFlightFences.clear();
}

void VulkanSync::WaitForFence(uint32_t currentFrame)
{
    m_inFlightFences[currentFrame]->Wait();
}

VulkanSync::VulkanSync(VulkanSync &&other) noexcept : 
    m_inFlightFences(std::move(other.m_inFlightFences)),
    m_imageSemaphores(std::move(other.m_imageSemaphores)),
    m_renderSemaphores(std::move(other.m_renderSemaphores))
{
    m_inFlightFences    = std::move(other.m_inFlightFences);
    m_imageSemaphores   = std::move(other.m_imageSemaphores);
    m_renderSemaphores  = std::move(other.m_renderSemaphores);
}

VulkanSync& VulkanSync::operator=(VulkanSync &&other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        m_inFlightFences   = std::move(other.m_inFlightFences);
        m_imageSemaphores  = std::move(other.m_imageSemaphores);
        m_renderSemaphores = std::move(other.m_renderSemaphores);
    }

    return *this;
}
