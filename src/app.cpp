#include "App.hpp"

#include <chrono>
#include <thread>

#include "Window/Window.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Camera.hpp"
#include "Renderer/Renderer.hpp"

App::App() = default;
App::~App()
{
    cleanup();
}

void App::init()
{
    // Window
    m_window = std::make_unique<Window>();
    m_window->init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME);

    // Camera
    std::unique_ptr<Camera> camera;
    camera = std::make_unique<Camera>(*m_window);
    camera->init(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, -90.0f, 0.0f), CAMERA_FOV, CAMERA_NEAR, CAMERA_FAR);

    // Scene
    std::unique_ptr<Scene> scene;
    scene = std::make_unique<Scene>();
    scene->init();
    scene->setCamera(camera);
    
    // Window
    m_renderer = std::make_unique<Renderer>(*m_window);
    m_renderer->init();
    m_renderer->setScene(scene);
}

void App::cleanup()
{
    m_renderer->cleanup();
    m_window->cleanup();
}

void App::run()
{
    double currentFrameTime = glfwGetTime();
    double lastFrameTime    = currentFrameTime;
    double deltaTime;

    while (!glfwWindowShouldClose(m_window->getHandle()))
    {
        currentFrameTime = glfwGetTime();
        deltaTime        = currentFrameTime - lastFrameTime;
        
        // Exit if ESC is Pressed
        if (glfwGetKey(m_window->getHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window->getHandle(), true);

        // Limit FPS
        if (deltaTime < FRAME_TIME)
        {
            double sleepTime = FRAME_TIME - deltaTime;
            if (sleepTime > 0.0)
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));

            currentFrameTime = glfwGetTime();
            deltaTime = currentFrameTime - lastFrameTime;
        }
        lastFrameTime = currentFrameTime;
        
        m_renderer->update(deltaTime);
        m_renderer->draw();

        glfwPollEvents();
    }
}
