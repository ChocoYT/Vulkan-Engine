#pragma once

#include <vulkan/vulkan.h>

#include "mesh.hpp"
#include "settings.hpp"
#include "vulkan_context.hpp"
#include "vulkan_pipeline.hpp"
#include "window.hpp"

class App
{
    public:
        void run();

    private:
        VulkanContext  context;
        VulkanPipeline pipeline;
        
        Window window{SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME};
};
