#include "app.hpp"

void App::run()
{
    context.init(window);
    pipeline.init(context);

    int currentFrame = 0;

    while (!glfwWindowShouldClose(window.getHandle()))
    {
        glfwPollEvents();
        pipeline.waitForFence(currentFrame);

        VkCommandBuffer commandBuffer = context.beginFrame(currentFrame);
        uint32_t imageIndex           = pipeline.beginFrame(commandBuffer, currentFrame);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        pipeline.endFrame(commandBuffer, imageIndex, currentFrame);

        currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
    }
    cleanup();
}

void App::cleanup()
{
    vkDeviceWaitIdle(context.getDevice());
    
    pipeline.cleanup();
    context.cleanup();
}
