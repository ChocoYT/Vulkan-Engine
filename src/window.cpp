#include "window.hpp"

void Window::init(int width, int height, const std::string &name)
{
    this->width  = width;
    this->height = height;
    this->name   = &name;

    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("GLFW not Initialized Correctly.");

    // Create GLFW Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!handle)
    {
        glfwTerminate();
        throw std::runtime_error("Window not Initialized Correctly.");
    }
}

void Window::cleanup()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}
