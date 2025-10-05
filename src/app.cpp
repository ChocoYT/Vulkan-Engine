#include "app.hpp"

void App::run()
{
    // Binding Description
    std::vector<VkVertexInputBindingDescription> bindingDescs(1);
    bindingDescs[0].binding   = 0;
    bindingDescs[0].stride    = sizeof(Vertex);
    bindingDescs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute Description
    std::vector<VkVertexInputAttributeDescription> attrDescs(2);
    attrDescs[0].binding  = 0;  // Position
    attrDescs[0].location = 0;
    attrDescs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[0].offset   = offsetof(Vertex, pos);

    attrDescs[1].binding  = 0;  // Color
    attrDescs[1].location = 1;
    attrDescs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[1].offset   = offsetof(Vertex, color);

    // Initialize
    context.init(window);
    pipeline.init(context, bindingDescs, attrDescs);

    // Initialize Mesh
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3( 0.0f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    std::vector<uint32_t> indices = {
        0, 1, 2
    };
    Mesh mesh;
    mesh.init(context, vertices, indices);

    int currentFrame = 0;

    while (!glfwWindowShouldClose(window.getHandle()))
    {
        glfwPollEvents();
        pipeline.waitForFence(currentFrame);

        VkCommandBuffer commandBuffer = context.beginFrame(currentFrame);
        uint32_t        imageIndex    = pipeline.beginFrame(commandBuffer, currentFrame);

        // Render
        pipeline.bind(commandBuffer);
        mesh.bind(commandBuffer);
        mesh.draw(commandBuffer);

        pipeline.endFrame(commandBuffer, imageIndex, currentFrame);

        currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
    }

    vkDeviceWaitIdle(context.getDevice());
    
    // Cleanup
    pipeline.cleanup();
    mesh.cleanup();

    context.cleanup();
}
