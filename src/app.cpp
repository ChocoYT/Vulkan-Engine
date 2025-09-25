#include "app.hpp"

void App::run()
{
    context.init(window);

    while (!glfwWindowShouldClose(window.getHandle()))
    {
        glfwPollEvents();
    }

    context.cleanup();
}
