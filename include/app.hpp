#pragma once

#include <vulkan/vulkan.h>

#include <chrono>
#include <thread>

#include "camera.hpp"
#include "mesh.hpp"
#include "settings.hpp"
#include "vulkan_context.hpp"
#include "vulkan_pipeline.hpp"
#include "window.hpp"

#include "Buffers/uniform_buffer.hpp"

class App
{
    public:
        void run();

    private:
        Window         window;
        VulkanContext  context;
        VulkanPipeline pipeline;
};
