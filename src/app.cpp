#include "app.hpp"

#include "Core/physical_device.hpp"
#include "Core/device.hpp"
#include "Core/command_pool.hpp"
#include "Core/descriptor_pool.hpp"

#include "settings.hpp"

void App::run()
{
    // Initialize Window and Context
    window.init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME);
    context.init(window.getHandle(), FRAMES_IN_FLIGHT);

    // Binding Description
    std::vector<VkVertexInputBindingDescription> bindingDescs(1);
    bindingDescs[0].binding   = 0;  // Vertex
    bindingDescs[0].stride    = sizeof(Vertex);
    bindingDescs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Attribute Description
    std::vector<VkVertexInputAttributeDescription> attrDescs(2);
    attrDescs[0].binding  = 0;  // Vertex Position
    attrDescs[0].location = 0;
    attrDescs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[0].offset   = offsetof(Vertex, pos);

    attrDescs[1].binding  = 0;  // Vertex Color
    attrDescs[1].location = 1;
    attrDescs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[1].offset   = offsetof(Vertex, color);

    // UBOs
    struct CameraBuffer {
        glm::mat4x4 cameraMatrix;
    };
    CameraBuffer cameraBufferData{};
    VkDeviceSize cameraBufferSize = sizeof(CameraBuffer);

    UniformBuffer cameraUBO(context);
    cameraUBO.init(cameraBufferSize, FRAMES_IN_FLIGHT);

    std::vector<VkDescriptorSetLayout> layoutDescs;
    layoutDescs.push_back(cameraUBO.getDescriptorSetLayout());

    // Initialize Pipeline
    pipeline.init(context, bindingDescs, attrDescs, layoutDescs, FRAMES_IN_FLIGHT);

    Camera camera(window);
    camera.init(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, -90.0f, 0.0f), CAMERA_FOV, CAMERA_NEAR, CAMERA_FAR);

    // Initialize Mesh
    std::vector<Vertex> vertices = {
        Vertex(glm::vec3( 0.0f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    std::vector<uint32_t> indices = {
        0, 1, 2
    };
    Mesh mesh(context);
    mesh.init(vertices, indices, FRAMES_IN_FLIGHT);

    uint32_t currentFrame = 0;

    double currentFrameTime = glfwGetTime();
    double lastFrameTime    = currentFrameTime;
    double deltaTime;

    while (!glfwWindowShouldClose(window.getHandle()))
    {
        currentFrameTime = glfwGetTime();
        deltaTime        = currentFrameTime - lastFrameTime;
        
        // Exit if ESC is Pressed
        if (glfwGetKey(window.getHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window.getHandle(), true);

        // Limit FPS
        if (deltaTime < FRAME_TIME)
        {
            double sleepTime = FRAME_TIME - deltaTime;
            if (sleepTime > 0.0)
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));

            currentFrameTime = glfwGetTime();
            deltaTime = currentFrameTime - lastFrameTime;
        }
        lastFrameTime = currentFrameTime;
        
        // Render
        pipeline.waitForFence(currentFrame);

        VkCommandBuffer commandBuffer = context.beginFrame(currentFrame);
        uint32_t        imageIndex    = pipeline.beginFrame(commandBuffer, currentFrame);
        
        pipeline.bind(commandBuffer);

        // Update Camera
        camera.update(deltaTime);

        // Uniform Buffers
        cameraBufferData.cameraMatrix = camera.getProjMatrix() * camera.getViewMatrix();

        cameraUBO.update(&cameraBufferData, currentFrame);
        cameraUBO.bind(commandBuffer, pipeline.getPipelineLayout(), currentFrame);
    
        // Draw Mesh
        mesh.bind(commandBuffer, currentFrame);
        mesh.draw(commandBuffer);

        pipeline.endFrame(commandBuffer, imageIndex, currentFrame);

        currentFrame  = (currentFrame + 1) % FRAMES_IN_FLIGHT;

        glfwPollEvents();
    }

    vkDeviceWaitIdle(context.getDevice().getHandle());
    
    // Cleanup
    pipeline.cleanup();
}
