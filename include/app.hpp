#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "settings.hpp"
#include "vulkan_context.hpp"
#include "window.hpp"

#ifndef APP_HPP
#define APP_HPP

class App
{
    public:
        void run();

    private:
        VulkanContext context;
        Window window{SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME};
};

#endif
