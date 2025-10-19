#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

class Instance;
class Debug;
class Surface;
class PhysicalDevice;
class Device;
class Swapchain;
class RenderPass;
class CommandPool;
class DescriptorPool;

class Context
{
    public:
        Context();
        ~Context();

        void init(
            GLFWwindow *window,
            uint32_t   frameCount
        );
        void cleanup();

        VkCommandBuffer beginFrame(int currentFrame);

        // Getters
        const Instance&       getInstance()       const { return *m_instance;       }
        const Surface&        getSurface()        const { return *m_surface;        }
        const PhysicalDevice& getPhysicalDevice() const { return *m_physicalDevice; }
        const Device&         getDevice()         const { return *m_device;         }
        const Swapchain&      getSwapchain()      const { return *m_swapchain;      }
        const RenderPass&     getRenderPass()     const { return *m_renderPass;     }
        const CommandPool&    getCommandPool()    const { return *m_commandPool;    }
        const DescriptorPool& getDescriptorPool() const { return *m_descriptorPool; }

    private:
        // Objects
        std::unique_ptr<Instance>       m_instance;
        std::unique_ptr<Debug>          m_debug;
        std::unique_ptr<Surface>        m_surface;
        std::unique_ptr<PhysicalDevice> m_physicalDevice;
        std::unique_ptr<Device>         m_device;
        std::unique_ptr<Swapchain>      m_swapchain;
        std::unique_ptr<RenderPass>     m_renderPass;
        std::unique_ptr<CommandPool>    m_commandPool;
        std::unique_ptr<DescriptorPool> m_descriptorPool;
};
