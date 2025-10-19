#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.hpp"

class Window;

class Camera
{
    public:
        Camera(Window &window);
        ~Camera();
;
        void init(glm::vec3 pos, glm::vec3 rot, float FOV, float NEAR, float FAR);
        
        void update(double deltaTime);
        void move(double deltaTime);
        void rotate(double deltaTime);

        glm::mat4x4 getProjMatrix();
        glm::mat4x4 getViewMatrix();
    
    private:
        void updateVectors();

        glm::vec3 m_position, m_rotation;
        glm::vec3 m_front, m_up, m_right;
        float m_fov, m_near, m_far;

        bool m_firstClick = true;
        glm::vec2 m_lastMousePosition;

        Window &m_window;
};
