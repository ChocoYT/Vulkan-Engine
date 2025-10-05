#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "utils.hpp"
#include "vulkan_context.hpp"

class VulkanPipeline
{
    public:
        void init(VulkanContext &vulkanContext);
        void cleanup();

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

        VkPipeline       graphicsPipeline       = VK_NULL_HANDLE;
        VkPipelineLayout graphicsPipelineLayout = VK_NULL_HANDLE;

        VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
        VkShaderModule pixelShaderModule  = VK_NULL_HANDLE;

    private:
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence>     inFlightFences;

        void createGraphicsPipeline();
        void createSemaphores();
        void createFences();

        VkShaderModule createShaderModule(const std::string &filePath);
        VkSemaphore createSemaphore(VkSemaphoreType type);
        VkFence createFence(VkFenceCreateFlags flags);

        VulkanContext *context;
};
