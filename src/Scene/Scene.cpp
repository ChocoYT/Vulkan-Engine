#include "Scene/Scene.hpp"

#include "Scene/Camera.hpp"
#include "Scene/Mesh.hpp"

VulkanScene::VulkanScene(
    std::unique_ptr<Camera> camera,
    std::vector<std::vector<VkDescriptorSet>> descriptorSets,
    std::vector<VkDescriptorSetLayout>        descriptorSetLayouts
) : m_camera(std::move(camera)),
    m_descriptorSets(std::move(descriptorSets)),
    m_descriptorSetLayouts(std::move(descriptorSetLayouts))
{};

VulkanScene::~VulkanScene()
{
    Cleanup();
}

std::unique_ptr<VulkanScene> VulkanScene::Create(
    const VulkanPhysicalDevice  &physicalDevice,
    const VulkanDevice          &device,
    const VulkanDescriptorPool  &descriptorPool,
    VulkanMemoryAllocator       &allocator,
    uint32_t frameCount
) {
    // Camera
    auto camera = Camera::Create(
        physicalDevice,
        device,
        descriptorPool,
        allocator,
        glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, -90.0f, 0.0f),
        CAMERA_FOV, CAMERA_NEAR, CAMERA_FAR
    );

    // Descriptor Sets
    std::vector<std::vector<VkDescriptorSet>> descriptorSets;
    for (uint32_t currentFrame = 0; currentFrame < frameCount; ++currentFrame)
    {
        std::vector<VkDescriptorSet> frameDescriptorSets;
        frameDescriptorSets.emplace_back(camera->GetDescriptorSet(currentFrame));

        descriptorSets.emplace_back(std::move(frameDescriptorSets));
    }

    // Descriptor Set Layouts
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.emplace_back(camera->GetDescriptorSetLayout());

    return std::unique_ptr<VulkanScene>(
        new VulkanScene(
            std::move(camera),
            std::move(descriptorSets),
            std::move(descriptorSetLayouts)
        )
    );
}

void VulkanScene::Cleanup()
{
    m_meshes.clear();

    m_descriptorSets.clear();
    m_descriptorSetLayouts.clear();
}

void VulkanScene::Update(
    const Window &window,
    double   deltaTime,
    uint32_t currentFrame
) {
    // Update Camera
    m_camera->Update(window, deltaTime);
    m_camera->UpdateBuffer(window, currentFrame);
}

VulkanScene::VulkanScene(VulkanScene&& other) noexcept : 
    m_camera(std::move(other.m_camera)),
    m_meshes(std::move(other.m_meshes)),
    m_descriptorSets(std::move(other.m_descriptorSets)),
    m_descriptorSetLayouts(std::move(other.m_descriptorSetLayouts))
{
    other = VulkanScene{};
}

VulkanScene& VulkanScene::operator=(VulkanScene &&other) noexcept
{
    if (this != &other)
    {
        Cleanup(); 

        m_camera = std::move(other.m_camera);
        m_meshes = std::move(other.m_meshes);
        m_descriptorSets       = std::move(other.m_descriptorSets);
        m_descriptorSetLayouts = std::move(other.m_descriptorSetLayouts);

        other = VulkanScene{};
    }
    return *this;
}
