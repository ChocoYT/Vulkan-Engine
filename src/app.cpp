#include "app.hpp"

void App::run()
{
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();
    }
}