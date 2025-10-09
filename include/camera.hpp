#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.hpp"
#include "window.hpp"

class Camera
{
    public:
        Camera(Window &window, glm::vec3 pos, glm::vec3 rot, float FOV, float NEAR, float FAR);
        
        void move(double deltaTime);
        void rotate(double deltaTime);

        glm::mat4x4 getProjMatrix();
        glm::mat4x4 getViewMatrix();

        glm::vec3 position, rotation;
        glm::vec3 front, up, right;
        float FOV, NEAR, FAR;
    
    private:
        void updateVectors();

        bool firstClick = true;
        glm::vec2 lastMousePosition;

        Window* window;
};
