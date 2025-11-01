#include "Scene/Camera.hpp"

#include "Window/Window.hpp"

#include "Vulkan/Buffers/Uniform.hpp"

Camera::Camera(
    glm::vec3 pos,
    glm::vec3 rot,
    float FOV,
    float NEAR,
    float FAR,
    std::unique_ptr<VulkanUniformBuffer> buffer
) : m_position(pos), 
    m_rotation(rot), 
    m_fov(glm::radians(FOV)), 
    m_near(NEAR), 
    m_far(FAR),
    m_buffer(std::move(buffer))
{
    UpdateVectors();
}

Camera::~Camera() = default;

std::unique_ptr<Camera> Camera::Create(
    const VulkanPhysicalDevice &physicalDevice,
    const VulkanDevice         &device,
    const VulkanDescriptorPool &descriptorPool,
    VulkanMemoryAllocator      &allocator,
    glm::vec3 pos,
    glm::vec3 rot,
    float FOV,
    float NEAR,
    float FAR
) {
    auto buffer = VulkanUniformBuffer::Create(
        physicalDevice,
        device,
        descriptorPool,
        allocator,
        sizeof(CameraBuffer),
        FRAMES_IN_FLIGHT
    );

    return std::unique_ptr<Camera>(new Camera(
        pos, rot,
        FOV, NEAR, FAR,
        std::move(buffer)
    ));
}

void Camera::Update(
    const Window &window,
    double deltaTime
) {
    Rotate(window, deltaTime);
    UpdateVectors();
    Move(window, deltaTime);
}

void Camera::Move(
    const Window &window,
    double deltaTime
) {
    int moveRight = (glfwGetKey(window.GetHandle(), GLFW_KEY_D) == GLFW_PRESS) - (glfwGetKey(window.GetHandle(), GLFW_KEY_A) == GLFW_PRESS);
    int moveUp    = (glfwGetKey(window.GetHandle(), GLFW_KEY_Q) == GLFW_PRESS) - (glfwGetKey(window.GetHandle(), GLFW_KEY_E) == GLFW_PRESS);
    int moveFront = (glfwGetKey(window.GetHandle(), GLFW_KEY_W) == GLFW_PRESS) - (glfwGetKey(window.GetHandle(), GLFW_KEY_S) == GLFW_PRESS);
    glm::vec3 moveVector = glm::normalize(glm::vec3(moveRight, moveUp, moveFront)) * CAMERA_MOVE_SPEED * static_cast<float>(deltaTime);

    if (moveFront != 0 || moveRight != 0 || moveUp != 0)
    {
        m_position += m_right * moveVector.x + m_up * moveVector.y + m_front * moveVector.z;
    }
}

void Camera::Rotate(
    const Window &window,
    double deltaTime
) {
    if (glfwGetMouseButton(window.GetHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if (m_firstClick)
		{
			glfwSetCursorPos(window.GetHandle(), SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
			m_firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window.GetHandle(), &mouseX, &mouseY);

		float rotX = CAMERA_SENSITIVITY * (static_cast<float>(mouseY) - SCREEN_HEIGHT * 0.5f) / SCREEN_HEIGHT;
		float rotY = CAMERA_SENSITIVITY * (static_cast<float>(mouseX) - SCREEN_WIDTH  * 0.5f) / SCREEN_WIDTH;
        
        m_rotation += glm::vec3(rotX, rotY, 0.0f) * CAMERA_SENSITIVITY * static_cast<float>(deltaTime);

		if      (m_rotation.x >  89.0f) m_rotation.x =  89.0f;
        else if (m_rotation.x < -89.0f) m_rotation.x = -89.0;

		glfwSetCursorPos(window.GetHandle(), SCREEN_WIDTH * 0.5, SCREEN_HEIGHT * 0.5);
	}
	else if (glfwGetMouseButton(window.GetHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_firstClick = true;
	}
}

glm::mat4x4 Camera::GetProjMatrix(const Window &window)
{
    int width, height;
    glfwGetFramebufferSize(window.GetHandle(), &width, &height);
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    
    return glm::perspective(m_fov, aspectRatio, m_near, m_far);
}

glm::mat4x4 Camera::GetViewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::UpdateVectors()
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

void Camera::UpdateBuffer(
    const Window &window,
    uint32_t currentFrame
) {
    m_bufferData.cameraMatrix = GetProjMatrix(window) * GetViewMatrix();
    m_buffer->Update(&m_bufferData, currentFrame);
}

void Camera::BindBuffer(
    VulkanPipeline &pipeline,
    VkCommandBuffer commandBuffer, 
    uint32_t        currentFrame
) {
    m_buffer->Bind(pipeline, commandBuffer, currentFrame);
}

Camera::Camera(Camera&& other) noexcept : 
    m_position(other.m_position),
    m_rotation(other.m_rotation),
    m_front(other.m_front),
    m_up(other.m_up),
    m_right(other.m_right),
    m_fov(other.m_fov),
    m_near(other.m_near),
    m_far(other.m_far),
    m_firstClick(other.m_firstClick),
    m_lastMousePosition(other.m_lastMousePosition),
    m_bufferData(other.m_bufferData),
    m_bufferSize(other.m_bufferSize),
    m_buffer(std::move(other.m_buffer))
{
    other = Camera{};
}

Camera& Camera::operator=(Camera &&other) noexcept
{
    if (this != &other)
    {
        m_position = other.m_position;
        m_rotation = other.m_rotation;
        m_front    = other.m_front;
        m_up       = other.m_up;
        m_right    = other.m_right;
        m_fov      = other.m_fov;
        m_near     = other.m_near;
        m_far      = other.m_far;
        m_firstClick        = other.m_firstClick;
        m_lastMousePosition = other.m_lastMousePosition;
        m_bufferData = other.m_bufferData;
        m_bufferSize = other.m_bufferSize;
        m_buffer     = std::move(other.m_buffer);

        other = Camera{};
    }

    return *this;
}
