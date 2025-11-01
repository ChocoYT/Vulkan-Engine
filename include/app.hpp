#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Settings.hpp"

class Window;
class VulkanScene;
class VulkanRenderer;

class App
{
    public:
        ~App();

        static App Create();

        void Run();

    private:
        App(
            std::unique_ptr<Window>         window,
            std::unique_ptr<VulkanRenderer> renderer,
            std::shared_ptr<VulkanScene>    scene
        );

        std::unique_ptr<Window>         m_window;
        std::unique_ptr<VulkanRenderer> m_renderer;
        std::shared_ptr<VulkanScene>    m_scene;
};
