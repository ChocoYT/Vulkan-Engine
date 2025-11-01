#pragma once

#include <cstdint>
#include <memory>

#include "Settings.hpp"

class Window;
class VulkanScene;

class VulkanContext;
class VulkanMemoryAllocator;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanSync;
class VulkanCommandPool;
class VulkanDescriptorPool;
class VulkanPipeline;

class VulkanRenderer
{
    public:
        ~VulkanRenderer();

        static std::unique_ptr<VulkanRenderer> Create(const Window &window);

        void Finish();
        void Draw();

        // Setters
        void SetScene(std::shared_ptr<VulkanScene> scene);

        // Getters
        const VulkanContext&         GetContext()         const { return *m_context; }
        const VulkanSwapchain&       GetSwapchain()       const { return *m_swapchain; }
        const VulkanRenderPass&      GetRenderPass()      const { return *m_renderPass; }
        const VulkanSync&            GetSync()            const { return *m_sync; }
        const VulkanCommandPool&     GetCommandPool()     const { return *m_commandPool; }
        const VulkanDescriptorPool&  GetDescriptorPool()  const { return *m_descriptorPool; }
        const VulkanPipeline&        GetPipeline()        const { return *m_pipeline; }
        VulkanMemoryAllocator&       GetAllocator()       const { return *m_allocator; }

        const uint32_t GetCurrentFrame() const { return m_currentFrame; }

    private:
        VulkanRenderer() = default;
        VulkanRenderer(
            std::unique_ptr<VulkanContext>         context,
            std::unique_ptr<VulkanMemoryAllocator> allocator,
            std::unique_ptr<VulkanSwapchain>       swapchain,
            std::unique_ptr<VulkanRenderPass>      renderPass,
            std::unique_ptr<VulkanSync>            sync,
            std::unique_ptr<VulkanCommandPool>     commandPool,
            std::unique_ptr<VulkanDescriptorPool>  descriptorPool
        );

        // Remove Copying Semantics
        VulkanRenderer(const VulkanRenderer&) = delete;
        VulkanRenderer& operator=(const VulkanRenderer&) = delete;
        
        // Safe Move Semantics
        VulkanRenderer(VulkanRenderer &&other) noexcept;
        VulkanRenderer& operator=(VulkanRenderer &&other) noexcept;

        // Objects
        std::unique_ptr<VulkanContext>         m_context;
        std::unique_ptr<VulkanMemoryAllocator> m_allocator;
        std::unique_ptr<VulkanSwapchain>       m_swapchain;
        std::unique_ptr<VulkanRenderPass>      m_renderPass;
        std::unique_ptr<VulkanSync>            m_sync;
        std::unique_ptr<VulkanCommandPool>     m_commandPool;
        std::unique_ptr<VulkanDescriptorPool>  m_descriptorPool;
        std::unique_ptr<VulkanPipeline>        m_pipeline;

        std::shared_ptr<VulkanScene> m_scene;

        uint32_t m_currentFrame = 0;
};