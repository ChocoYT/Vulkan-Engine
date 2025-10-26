#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "Settings.hpp"

class Renderer;

class Camera;
class Mesh;

class UniformBuffer;

class Scene
{
    public:
        Scene();
        ~Scene();

        void init();
        void cleanup();
        
        void update(double deltaTime);

        // Setters
        void setCamera(std::unique_ptr<Camera>& camera) { m_camera = std::move(camera); }

        // Getters
        const std::unique_ptr<Camera>&            getCamera() const { return m_camera; }
        const std::vector<std::unique_ptr<Mesh>>& getMeshes() const { return m_meshes; }

    private:
        std::vector<std::unique_ptr<Mesh>> m_meshes;

        std::unique_ptr<Camera> m_camera;
};
