#include "App.hpp"

#include <chrono>
#include <thread>

#include "Window/Window.hpp"
#include "Scene/Scene.hpp"
#include "Vulkan/Renderer/Renderer.hpp"
#include "Vulkan/Core/Context.hpp"

#include "Scene/Mesh.hpp"

App::App(
    std::unique_ptr<Window>         window,
    std::unique_ptr<VulkanRenderer> renderer,
    std::shared_ptr<VulkanScene>    scene
) : m_window(std::move(window)),
    m_renderer(std::move(renderer)),
    m_scene(std::move(scene))
{}

App::~App() = default;

App App::Create()
{
    // Window
    auto window = Window::Create();
    
    // Renderer
    auto renderer = VulkanRenderer::Create(*window);

    // Scene
    std::shared_ptr<VulkanScene> scene = std::move(VulkanScene::Create(
        renderer->GetContext().GetPhysicalDevice(),
        renderer->GetContext().GetDevice(),
        renderer->GetDescriptorPool(),
        renderer->GetAllocator(),
        FRAMES_IN_FLIGHT
    ));

    renderer->SetScene(scene);

    return App(
        std::move(window),
        std::move(renderer),
        std::move(scene)
    );
}

void App::Run()
{
    double currentFrameTime = glfwGetTime();
    double lastFrameTime    = currentFrameTime;
    double deltaTime;

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3( 0.0f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    std::vector<uint32_t> indices = {
        0, 1, 2
    };

    auto mesh = VulkanMesh::Create(
        m_renderer->GetContext().GetPhysicalDevice(),
        m_renderer->GetContext().GetDevice(),
        m_renderer->GetAllocator(),
        vertices.size() * sizeof(Vertex),
        indices.size()  * sizeof(uint32_t),
        FRAMES_IN_FLIGHT
    );

    for (uint32_t currentFrame = 0; currentFrame < FRAMES_IN_FLIGHT; ++currentFrame)
    {
        mesh->UpdateBuffers(
            m_renderer->GetCommandPool(),
            vertices,
            indices,
            currentFrame
        );
    }

    m_scene->AddMesh(std::move(mesh));

    while (!glfwWindowShouldClose(m_window->GetHandle()))
    {
        currentFrameTime = glfwGetTime();
        deltaTime        = currentFrameTime - lastFrameTime;
        
        // Exit if ESC is Pressed
        if (glfwGetKey(m_window->GetHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window->GetHandle(), true);

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
        
        m_scene->Update(
            *m_window,
            deltaTime,
            m_renderer->GetCurrentFrame()
        );
        m_renderer->Draw();

        glfwPollEvents();
    }

    m_renderer->Finish();
}
