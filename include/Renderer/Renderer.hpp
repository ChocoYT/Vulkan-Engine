#pragma once

#include <cstdint>
#include <memory>

#include "Settings.hpp"

class Window;
class Scene;

class Context;
class MemoryAllocator;
class Swapchain;
class RenderPass;
class Synchronization;
class CommandPool;
class DescriptorPool;
class Pipeline;

class Renderer
{
    public:
        Renderer(
            const Window &window,
            std::unique_ptr<Scene> scene
        );
        ~Renderer();

        void init();
        void cleanup();

        void update(double deltaTime);
        void draw();

        // Setters
        void setScene(std::unique_ptr<Scene>& scene) { m_scene = std::move(scene); }

        // Getters
        const std::unique_ptr<Context>&         getContext()         const { return m_context; }
        const std::unique_ptr<MemoryAllocator>& getAllocator()       const { return m_allocator; }
        const std::unique_ptr<Swapchain>&       getSwapchain()       const { return m_swapchain; }
        const std::unique_ptr<RenderPass>&      getRenderPass()      const { return m_renderPass; }
        const std::unique_ptr<Synchronization>& getSynchronization() const { return m_synchronization; }
        const std::unique_ptr<CommandPool>&     getCommandPool()     const { return m_commandPool; }
        const std::unique_ptr<DescriptorPool>&  getDescriptorPool()  const { return m_descriptorPool; }
        const std::unique_ptr<Pipeline>&        getPipeline()        const { return m_pipeline; }

        const uint32_t getCurrentFrame() const { return m_currentFrame; }

    private:
        std::unique_ptr<Context>         m_context;
        std::unique_ptr<MemoryAllocator> m_allocator;
        std::unique_ptr<Swapchain>       m_swapchain;
        std::unique_ptr<RenderPass>      m_renderPass;
        std::unique_ptr<Synchronization> m_synchronization;
        std::unique_ptr<CommandPool>     m_commandPool;
        std::unique_ptr<DescriptorPool>  m_descriptorPool;
        std::unique_ptr<Pipeline>        m_pipeline;
        
        const Window &m_window;
        std::unique_ptr<Scene> m_scene;

        uint32_t m_currentFrame = 0;
};