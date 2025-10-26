#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

class Window;

class Instance;
class Debug;
class Surface;
class PhysicalDevice;
class Device;

class Context
{
    public:
        Context(const Window &window);
        ~Context();

        void init(uint32_t frameCount);
        void cleanup();

        // Getters
        const Instance&       getInstance()       const { return *m_instance;       }
        const Surface&        getSurface()        const { return *m_surface;        }
        const PhysicalDevice& getPhysicalDevice() const { return *m_physicalDevice; }
        const Device&         getDevice()         const { return *m_device;         }

    private:
        // Objects
        std::unique_ptr<Instance>       m_instance;
        std::unique_ptr<Debug>          m_debug;
        std::unique_ptr<Surface>        m_surface;
        std::unique_ptr<PhysicalDevice> m_physicalDevice;
        std::unique_ptr<Device>         m_device;

        const Window &m_window;
};
