#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Settings.hpp"

class Window;
class Context;
class CommandPool;
class DescriptorPool;
class MemoryAllocator;

class UniformBuffer;

struct CameraBuffer {
    glm::mat4x4 cameraMatrix;
};

class Camera
{
    public:
        Camera(const Window &window);
        ~Camera();
;
        void init(glm::vec3 pos, glm::vec3 rot, float FOV, float NEAR, float FAR);
        void initBuffer(
            const Context        &context,
            const CommandPool    &commandPool,
            const DescriptorPool &descriptorPool,
            MemoryAllocator      &memoryAllocator
        );
        void cleanup();
        
        void update(double deltaTime);

        void updateBuffer(uint32_t currentFrame);
        void bindBuffer(
            VkCommandBuffer commandBuffer, 
            VkPipelineLayout pipelineLayout, 
            uint32_t currentFrame
        );

        glm::mat4x4 getProjMatrix();
        glm::mat4x4 getViewMatrix();

        const std::unique_ptr<UniformBuffer>& getBuffer() const { return m_buffer; }
    
    private:
        void move(double deltaTime);
        void rotate(double deltaTime);
        void updateVectors();

        glm::vec3 m_position, m_rotation;
        glm::vec3 m_front, m_up, m_right;
        float m_fov, m_near, m_far;

        bool m_firstClick = true;
        glm::vec2 m_lastMousePosition;

        CameraBuffer m_bufferData{};
        VkDeviceSize m_bufferSize = sizeof(CameraBuffer);
        std::unique_ptr<UniformBuffer> m_buffer;

        const Window &m_window;
};
