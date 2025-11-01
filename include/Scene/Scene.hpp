#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "Settings.hpp"

class Window;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanMemoryAllocator;
class VulkanDescriptorPool;

class Camera;
class VulkanMesh;

class VulkanScene
{
    public:
        ~VulkanScene();

        static std::unique_ptr<VulkanScene> Create(
            const VulkanPhysicalDevice  &physicalDevice,
            const VulkanDevice          &device,
            const VulkanDescriptorPool  &descriptorPool,
            VulkanMemoryAllocator       &allocator,
            uint32_t frameCount
        );
        
        void Update(
            const Window &window,
            double   deltaTime,
            uint32_t currentFrame
        );
        
        void AddMesh(std::unique_ptr<VulkanMesh> mesh)
        {
            m_meshes.emplace_back(std::move(mesh));
        }

        // Getters
        const std::unique_ptr<Camera>&                  GetCamera() const { return m_camera; }
        const std::vector<std::unique_ptr<VulkanMesh>>& GetMeshes() const { return m_meshes; }

        const std::vector<VkDescriptorSet>&       GetDescriptorSets(uint32_t currentFrame) const { return m_descriptorSets[currentFrame]; }
        const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts()                const { return m_descriptorSetLayouts; }

    private:
        VulkanScene() = default;
        VulkanScene(
            std::unique_ptr<Camera> camera,
            std::vector<std::vector<VkDescriptorSet>> descriptorSets,
            std::vector<VkDescriptorSetLayout>        descriptorSetLayouts
        );

        // Remove Copying Semantics
        VulkanScene(const VulkanScene&) = delete;
        VulkanScene& operator=(const VulkanScene&) = delete;
        
        // Safe Move Semantics
        VulkanScene(VulkanScene &&other) noexcept;
        VulkanScene& operator=(VulkanScene &&other) noexcept;
        
        void Cleanup();

        std::unique_ptr<Camera>                  m_camera;
        std::vector<std::unique_ptr<VulkanMesh>> m_meshes;

        std::vector<std::vector<VkDescriptorSet>> m_descriptorSets;
        std::vector<VkDescriptorSetLayout>        m_descriptorSetLayouts;
};
