#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "Vulkan/Buffers/Uniform.hpp"

#include "Settings.hpp"

class Window;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanDescriptorPool;
class VulkanPipeline;

class VulkanUniformBuffer;

struct CameraBuffer {
    glm::mat4x4 cameraMatrix;
};

class Camera
{
    public:
        ~Camera();
;
        static std::unique_ptr<Camera> Create(
            const VulkanPhysicalDevice &physicalDevice,
            const VulkanDevice         &device,
            const VulkanDescriptorPool &descriptorPool,
            VulkanMemoryAllocator      &allocator,
            glm::vec3 pos,
            glm::vec3 rot,
            float FOV,
            float NEAR,
            float FAR
        );
        
        void Update(
            const Window &window,
            double deltaTime
        );

        void UpdateBuffer(
            const Window &window,
            uint32_t currentFrame
        );
        void BindBuffer(
            VulkanPipeline &pipeline,
            VkCommandBuffer commandBuffer, 
            uint32_t        currentFrame
        );

        glm::mat4x4 GetProjMatrix(const Window &window);
        glm::mat4x4 GetViewMatrix();

        const std::unique_ptr<VulkanUniformBuffer>& GetBuffer() const { return m_buffer; }

        VkDescriptorSet       GetDescriptorSet(uint32_t currentFrame) const { return m_buffer->GetDescriptorSets(currentFrame); }
        VkDescriptorSetLayout GetDescriptorSetLayout()                const { return m_buffer->GetDescriptorSetLayout(); }
    
    private:
        Camera() = default;
        Camera(
            glm::vec3 pos,
            glm::vec3 rot,
            float FOV,
            float NEAR,
            float FAR,
            std::unique_ptr<VulkanUniformBuffer> buffer
        );

        // Remove Copying Semantics
        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;
        
        // Safe Move Semantics
        Camera(Camera &&other) noexcept;
        Camera& operator=(Camera &&other) noexcept;

        void Move(
            const Window &window,
            double deltaTime
        );
        void Rotate(
            const Window &window,
            double deltaTime
        );
        void UpdateVectors();

        glm::vec3 m_position, m_rotation = glm::vec3(0, 0, 0);
        glm::vec3 m_front, m_up, m_right = glm::vec3(0, 0, 0);
        float m_fov, m_near, m_far = 0;

        bool m_firstClick = true;
        glm::vec2 m_lastMousePosition = glm::vec2(0, 0);

        // Camera Buffer
        CameraBuffer m_bufferData{};
        VkDeviceSize m_bufferSize = sizeof(CameraBuffer);
        std::unique_ptr<VulkanUniformBuffer> m_buffer;
};
