#include "Vulkan/Core/Context.hpp"

#include "Window/Window.hpp"

#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/Debug.hpp"
#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

VulkanContext::VulkanContext(
    std::unique_ptr<VulkanInstance>       instance,
    std::unique_ptr<VulkanDebug>          debug,
    std::unique_ptr<VulkanSurface>        surface,
    std::unique_ptr<VulkanPhysicalDevice> physicalDevice,
    std::unique_ptr<VulkanDevice>         device
) : m_instance(std::move(instance)),
    m_debug(std::move(debug)),
    m_surface(std::move(surface)),
    m_physicalDevice(std::move(physicalDevice)),
    m_device(std::move(device))
{};

VulkanContext::~VulkanContext() = default;

std::unique_ptr<VulkanContext> VulkanContext::Create(
    const Window &window,
    uint32_t frameCount
) {
    auto instance = VulkanInstance::Create(
        SCREEN_NAME,
        VK_MAKE_VERSION(1, 0, 0),
        ENGINE_NAME,
        VK_MAKE_VERSION(1, 0, 0)
    );

    auto debug = VulkanDebug::Create(*instance);

    auto surface = VulkanSurface::Create(window, *instance);

    auto physicalDevice = VulkanPhysicalDevice::Create(*instance, *surface);

    auto device = VulkanDevice::Create(*physicalDevice);

    return std::unique_ptr<VulkanContext>(
        new VulkanContext(
            std::move(instance),
            std::move(debug),
            std::move(surface),
            std::move(physicalDevice),
            std::move(device)
        )
    );
}

VulkanContext::VulkanContext(VulkanContext &&other) noexcept : 
    m_instance(std::move(other.m_instance)),
    m_debug(std::move(other.m_debug)),
    m_surface(std::move(other.m_surface)),
    m_physicalDevice(std::move(other.m_physicalDevice)),
    m_device(std::move(other.m_device))
{}

VulkanContext& VulkanContext::operator=(VulkanContext &&other) noexcept
{
    if (this != &other)
    {
        m_instance       = std::move(other.m_instance);
        m_debug          = std::move(other.m_debug);
        m_surface        = std::move(other.m_surface);
        m_physicalDevice = std::move(other.m_physicalDevice);
        m_device         = std::move(other.m_device);
    }

    return *this;
}
