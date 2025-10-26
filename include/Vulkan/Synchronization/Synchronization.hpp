#pragma once

#include <cstdint>
#include <memory>
#include <vector>

class Context;

class Fence;
class Semaphore;

class Synchronization
{
    public:
        Synchronization(const Context &context);
        ~Synchronization();

        void init(uint32_t frameCount);
        void cleanup();

        void waitForFence(uint32_t currentFrame);

        // Getters
        const std::vector<std::unique_ptr<Semaphore>>& getImageSemaphores()  const { return m_imageSemaphores;  }
        const std::vector<std::unique_ptr<Semaphore>>& getRenderSemaphores() const { return m_renderSemaphores; }
        const std::vector<std::unique_ptr<Fence>>&     getInFlightFences()   const { return m_inFlightFences;   }

    private:
        // Objects
        std::vector<std::unique_ptr<Fence>>     m_inFlightFences;
        std::vector<std::unique_ptr<Semaphore>> m_imageSemaphores;
        std::vector<std::unique_ptr<Semaphore>> m_renderSemaphores;

        const Context &m_context;
};
