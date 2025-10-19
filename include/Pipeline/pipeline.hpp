#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

#include "mesh.hpp"
#include "utils.hpp"

class Context;

class Pipeline
{
    public:
        void init(
            Context &context,
            std::vector<VkVertexInputBindingDescription>   &bindingDescs,
            std::vector<VkVertexInputAttributeDescription> &attrDescs,
            std::vector<VkDescriptorSetLayout>             &layoutDescs,
            uint32_t frameCount
        );
        void cleanup();

        void bind(VkCommandBuffer commandBuffer);

        uint32_t beginFrame(VkCommandBuffer commandBuffer, int currentFrame);
        void     endFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, int currentFrame);

        void waitForFence(int currentFrame);

        // Pipeline Getters
        VkPipeline       getGraphicsPipeline() const { return graphicsPipeline;       }
        VkPipelineLayout getPipelineLayout()   const { return graphicsPipelineLayout; }

        // Synchronization Getters
        const std::vector<VkSemaphore>& getImageAvailableSemaphores() const { return imageAvailableSemaphores; }
        const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const { return renderFinishedSemaphores; }
        const std::vector<VkFence>&     getInFlightFences()           const { return inFlightFences;           }

    private:
        // Pipeline
        VkPipeline       graphicsPipeline       = VK_NULL_HANDLE;
        VkPipelineLayout graphicsPipelineLayout = VK_NULL_HANDLE;

        // Descriptions
        std::vector<VkVertexInputBindingDescription>   bindingDescs;
        std::vector<VkVertexInputAttributeDescription> attrDescs;
        std::vector<VkDescriptorSetLayout>             layoutDescs;

        // Shader Modules
        VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
        VkShaderModule pixelShaderModule  = VK_NULL_HANDLE;

        // Synchronization
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence>     inFlightFences;

        void createGraphicsPipeline();
        void createSemaphores(uint32_t frameCount);
        void createFences(uint32_t frameCount);

        VkShaderModule createShaderModule(const std::string &filePath);
        VkSemaphore createSemaphore(VkSemaphoreType type);
        VkFence createFence(VkFenceCreateFlags flags);

        const Context *m_context;
};
