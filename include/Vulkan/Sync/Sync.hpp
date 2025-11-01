#pragma once

#include <memory>
#include <vector>

class VulkanDevice;

class VulkanFence;
class VulkanSemaphore;

class VulkanSync
{
    public:
        ~VulkanSync();

        static std::unique_ptr<VulkanSync> Create(
            const VulkanDevice &device,
            uint32_t frameCount
        );

        void WaitForFence(uint32_t currentFrame);

        // Getters
        const std::vector<std::unique_ptr<VulkanFence>>&     GetInFlightFences()   const { return m_inFlightFences;   }
        const std::vector<std::unique_ptr<VulkanSemaphore>>& GetImageSemaphores()  const { return m_imageSemaphores;  }
        const std::vector<std::unique_ptr<VulkanSemaphore>>& GetRenderSemaphores() const { return m_renderSemaphores; }

    private:
        VulkanSync(
            std::vector<std::unique_ptr<VulkanFence>>     inFlightFences,
            std::vector<std::unique_ptr<VulkanSemaphore>> imageSemaphores,
            std::vector<std::unique_ptr<VulkanSemaphore>> renderSemaphores
        );

        void Cleanup();

        // Remove Copying Semantics
        VulkanSync(const VulkanSync&) = delete;
        VulkanSync& operator=(const VulkanSync&) = delete;
        
        // Safe Move Semantics
        VulkanSync(VulkanSync &&other) noexcept;
        VulkanSync& operator=(VulkanSync &&other) noexcept;

        // Objects
        std::vector<std::unique_ptr<VulkanFence>>     m_inFlightFences;
        std::vector<std::unique_ptr<VulkanSemaphore>> m_imageSemaphores;
        std::vector<std::unique_ptr<VulkanSemaphore>> m_renderSemaphores;
};
