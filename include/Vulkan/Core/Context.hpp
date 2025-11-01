#pragma once

#include <cstdint>
#include <memory>

#include <vulkan/vulkan.h>

#include "Settings.hpp"

class Window;

class VulkanInstance;
class VulkanDebug;
class VulkanSurface;
class VulkanPhysicalDevice;
class VulkanDevice;

class VulkanContext
{
    public:
        ~VulkanContext();

        static std::unique_ptr<VulkanContext> Create(
            const Window &window,
            uint32_t frameCount
        );

        // Getters
        const VulkanInstance&       GetInstance()       const { return *m_instance;       }
        const VulkanDebug&          GetDebug()          const { return *m_debug;          }
        const VulkanSurface&        GetSurface()        const { return *m_surface;        }
        const VulkanPhysicalDevice& GetPhysicalDevice() const { return *m_physicalDevice; }
        const VulkanDevice&         GetDevice()         const { return *m_device;         }

    private:
        VulkanContext(
            std::unique_ptr<VulkanInstance>       instance,
            std::unique_ptr<VulkanDebug>          debug,
            std::unique_ptr<VulkanSurface>        surface,
            std::unique_ptr<VulkanPhysicalDevice> physicalDevice,
            std::unique_ptr<VulkanDevice>         device
        );

        // Remove Copying Semantics
        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;
        
        // Safe Move Semantics
        VulkanContext(VulkanContext &&other) noexcept;
        VulkanContext& operator=(VulkanContext &&other) noexcept;

        // Objects
        std::unique_ptr<VulkanInstance>       m_instance;
        std::unique_ptr<VulkanDebug>          m_debug;
        std::unique_ptr<VulkanSurface>        m_surface;
        std::unique_ptr<VulkanPhysicalDevice> m_physicalDevice;
        std::unique_ptr<VulkanDevice>         m_device;
};
