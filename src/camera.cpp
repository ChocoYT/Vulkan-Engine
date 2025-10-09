#include "camera.hpp"

Camera::Camera(Window &window, glm::vec3 pos, glm::vec3 rot, float FOV, float NEAR, float FAR)
{   
    this->window = &window;

    this->position = pos;
    this->rotation = rot;

    this->FOV  = glm::radians(FOV);
    this->NEAR = NEAR;
    this->FAR  = FAR;

    updateVectors();
}

void Camera::move(double deltaTime)
{
    int moveRight = (glfwGetKey(window->getHandle(), GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window->getHandle(), GLFW_KEY_A) == GLFW_PRESS);
    int moveUp    = (glfwGetKey(window->getHandle(), GLFW_KEY_Q) == GLFW_PRESS) - (glfwGetKey(window->getHandle(), GLFW_KEY_E) == GLFW_PRESS);
    int moveFront = (glfwGetKey(window->getHandle(), GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(window->getHandle(), GLFW_KEY_S) == GLFW_PRESS);
    glm::vec3 moveVector = glm::normalize(glm::vec3(moveRight, moveUp, moveFront)) * CAMERA_MOVE_SPEED * static_cast<float>(deltaTime);

    if (moveFront != 0 || moveRight != 0 || moveUp != 0)
    {
        position += right * moveVector.x + up * moveVector.y + front * moveVector.z;
    }
}

void Camera::rotate(double deltaTime)
{
    if (glfwGetMouseButton(window->getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window->getHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if (firstClick)
		{
			glfwSetCursorPos(window->getHandle(), SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window->getHandle(), &mouseX, &mouseY);

		float rotX = CAMERA_SENSITIVITY * (static_cast<float>(mouseY) - SCREEN_HEIGHT * 0.5f) / SCREEN_HEIGHT;
		float rotY = CAMERA_SENSITIVITY * (static_cast<float>(mouseX) - SCREEN_WIDTH  * 0.5f) / SCREEN_WIDTH;
        
        rotation += glm::vec3(rotX, rotY, 0.0f) * CAMERA_SENSITIVITY * static_cast<float>(deltaTime);

		if      (rotation.x >  89.0f) rotation.x =  89.0f;
        else if (rotation.x < -89.0f) rotation.x = -89.0;

        updateVectors();

		glfwSetCursorPos(window->getHandle(), SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
	}
	else if (glfwGetMouseButton(window->getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}

glm::mat4x4 Camera::getProjMatrix()
{
    int width, height;
    glfwGetFramebufferSize(window->getHandle(), &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    return glm::perspective(FOV, aspectRatio, NEAR, FAR);
}

glm::mat4x4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::updateVectors()
{
    // Calculate New Front Vector
    front = glm::normalize(glm::vec3(
        cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)),
        sin(glm::radians(rotation.x)),
        cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y))
    ));

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Calculate Right and Up Vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
