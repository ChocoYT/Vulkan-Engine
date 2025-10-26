#include "Vulkan/Core/Context.hpp"

#include "window.hpp"

#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/Debug.hpp"
#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"

Context::Context(const Window &window) : m_window(window) {};
Context::~Context()
{
    cleanup();
}

void Context::init(uint32_t frameCount)
{
    // Instance
    m_instance = std::make_unique<Instance>();
    m_instance->init();

    // Debugger
    m_debug = std::make_unique<Debug>(*this);
    m_debug->init();

    // Surface
    m_surface = std::make_unique<Surface>(m_window, *this);
    m_surface->init();

    // Physical Device
    m_physicalDevice = std::make_unique<PhysicalDevice>(*this);
    m_physicalDevice->init();

    // Device
    m_device = std::make_unique<Device>(*this);
    m_device->init();
}

void Context::cleanup()
{
    m_device->cleanup();
    m_surface->cleanup();
    m_debug->cleanup();
    m_instance->cleanup();
}
