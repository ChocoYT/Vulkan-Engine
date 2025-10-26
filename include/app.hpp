#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "settings.hpp"

class Window;
class Renderer;

class App
{
    public:
        App();
        ~App();

        void init();
        void cleanup();

        void run();

    private:
        std::unique_ptr<Window>   m_window;
        std::unique_ptr<Renderer> m_renderer;
};
