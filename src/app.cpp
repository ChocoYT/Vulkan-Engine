#include "app.hpp"

void App::run()
{
    // Initialize Window and Context
    window.init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NAME);
    context.init(window);

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

    UniformBuffer cameraUBO;
    cameraUBO.init(context, cameraBufferSize);

    std::vector<VkDescriptorSetLayout> layoutDescs;
    layoutDescs.push_back(cameraUBO.getDescriptorSetLayout());

    // Initialize Pipeline
    pipeline.init(context, bindingDescs, attrDescs, layoutDescs);

    Camera camera(window, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, -90.0f, 0.0f), CAMERA_FOV, CAMERA_NEAR, CAMERA_FAR);

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

    double currentFrameTime = glfwGetTime();
    double lastFrameTime    = currentFrame;
    double deltaTime;

    while (!glfwWindowShouldClose(window.getHandle()))
    {
        currentFrameTime = glfwGetTime();
        deltaTime        = currentFrameTime - lastFrameTime;
        
        if (glfwGetKey(window.getHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window.getHandle(), true);

        if (deltaTime < FRAME_TIME)
        {
            double sleepTime = FRAME_TIME - deltaTime;
            if (sleepTime > 0.0)
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));

            currentFrameTime = glfwGetTime();
            deltaTime = currentFrameTime - lastFrameTime;
        }
        lastFrameTime = currentFrameTime;

        pipeline.waitForFence(currentFrame);

        VkCommandBuffer commandBuffer = context.beginFrame(currentFrame);
        uint32_t        imageIndex    = pipeline.beginFrame(commandBuffer, currentFrame);
        
        pipeline.bind(commandBuffer);

        // Update Camera
        camera.rotate(deltaTime);
        camera.move(deltaTime);

        // Uniform Buffers
        cameraBufferData.cameraMatrix = camera.getProjMatrix() * camera.getViewMatrix();

        cameraUBO.update(currentFrame, &cameraBufferData);
        cameraUBO.bind(context, pipeline, currentFrame);
    
        // Draw Mesh
        mesh.bind(commandBuffer);
        mesh.draw(commandBuffer);

        pipeline.endFrame(commandBuffer, imageIndex, currentFrame);

        currentFrame  = (currentFrame + 1) % FRAMES_IN_FLIGHT;

        glfwPollEvents();
    }

    vkDeviceWaitIdle(context.getDevice());
    
    // Cleanup
    pipeline.cleanup();

    mesh.cleanup();
    cameraUBO.cleanup(context);

    context.cleanup();
    window.cleanup();
}
