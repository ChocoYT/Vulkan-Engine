#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "settings.hpp"
#include "vulkan_context.hpp"
#include "vulkan_pipeline.hpp"
#include "window.hpp"

#ifndef APP_HPP
#define APP_HPP

class App
{
    public:
        void run();
        void cleanup();

    private:
        VulkanContext  context;
        VulkanPipeline pipeline;
        
        Window window{SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME};
};

#endif
