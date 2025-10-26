#include "Scene/Camera.hpp"

#include "Window/Window.hpp"

#include "Vulkan/Buffers/Uniform.hpp"

Camera::Camera(const Window &window) : m_window(window) {}
Camera::~Camera()
{
    cleanup();
}

void Camera::init(glm::vec3 position, glm::vec3 rotation, float fov, float near, float far)
{
    m_position = position;
    m_rotation = rotation;

    m_fov  = glm::radians(fov);
    m_near = near;
    m_far  = far;

    updateVectors();
}

void Camera::initBuffer(
    const Context        &context,
    const CommandPool    &commandPool,
    const DescriptorPool &descriptorPool,
    MemoryAllocator      &allocator
) {
    m_buffer = std::make_unique<UniformBuffer>(
        context,
        commandPool,
        descriptorPool,
        allocator
    );
    m_buffer->init(m_bufferSize, FRAMES_IN_FLIGHT);
}

void Camera::cleanup()
{
    m_buffer->cleanup();
}

void Camera::update(double deltaTime)
{
    rotate(deltaTime);
    updateVectors();
    move(deltaTime);
}

void Camera::move(double deltaTime)
{
    GLFWwindow *window = m_window.getHandle();

    int moveRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
    int moveUp    = (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
    int moveFront = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    glm::vec3 moveVector = glm::normalize(glm::vec3(moveRight, moveUp, moveFront)) * CAMERA_MOVE_SPEED * static_cast<float>(deltaTime);

    if (moveFront != 0 || moveRight != 0 || moveUp != 0)
    {
        m_position += m_right * moveVector.x + m_up * moveVector.y + m_front * moveVector.z;
    }
}

void Camera::rotate(double deltaTime)
{
    GLFWwindow *window = m_window.getHandle();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if (m_firstClick)
		{
			glfwSetCursorPos(window, SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
			m_firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = CAMERA_SENSITIVITY * (static_cast<float>(mouseY) - SCREEN_HEIGHT * 0.5f) / SCREEN_HEIGHT;
		float rotY = CAMERA_SENSITIVITY * (static_cast<float>(mouseX) - SCREEN_WIDTH  * 0.5f) / SCREEN_WIDTH;
        
        m_rotation += glm::vec3(rotX, rotY, 0.0f) * CAMERA_SENSITIVITY * static_cast<float>(deltaTime);

		if      (m_rotation.x >  89.0f) m_rotation.x =  89.0f;
        else if (m_rotation.x < -89.0f) m_rotation.x = -89.0;

		glfwSetCursorPos(window, SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_firstClick = true;
	}
}

glm::mat4x4 Camera::getProjMatrix()
{
    GLFWwindow *window = m_window.getHandle();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    return glm::perspective(m_fov, aspectRatio, m_near, m_far);
}

glm::mat4x4 Camera::getViewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::updateVectors()
{
    // Calculate New Front Vector
    m_front = glm::normalize(glm::vec3(
        cos(glm::radians(m_rotation.x)) * cos(glm::radians(m_rotation.y)),
        sin(glm::radians(m_rotation.x)),
        cos(glm::radians(m_rotation.x)) * sin(glm::radians(m_rotation.y))
    ));

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Calculate Right and Up Vectors
    m_right = glm::normalize(glm::cross(m_front, worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateBuffer(uint32_t currentFrame)
{
    m_bufferData.cameraMatrix = getProjMatrix() * getViewMatrix();
    m_buffer->update(&m_bufferData, currentFrame);
}

void Camera::bindBuffer(
    VkCommandBuffer commandBuffer, 
    VkPipelineLayout pipelineLayout, 
    uint32_t currentFrame
) {
    m_buffer->bind(commandBuffer, pipelineLayout, currentFrame);
}
