#include "Scene/Scene.hpp"

#include "Renderer/Renderer.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"

#include "Scene/Camera.hpp"
#include "Scene/Mesh.hpp"

Scene::Scene() = default;
Scene::~Scene()
{
    cleanup();
}

void Scene::init()
{
    std::unique_ptr<Mesh> mesh;
    mesh = std::make_unique<Mesh>();

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3( 0.0f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    std::vector<uint32_t> indices = {
        0, 1, 2
    };
    mesh->init(vertices, indices, FRAMES_IN_FLIGHT);

    m_meshes.emplace_back(mesh);
}

void Scene::cleanup()
{
    // Destroy Meshes
    for (auto &mesh : m_meshes)
        mesh->cleanup();

    m_meshes.clear();

    // Destroy Camera
    m_camera->cleanup();
}

void Scene::update(double deltaTime)
{
    // Update Camera
    m_camera->update(deltaTime);
}
