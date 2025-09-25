#include "window.hpp"

Window::Window(int width, int height, std::string name)
{
    this->width  = width;
    this->height = height;
    this->name   = name;

    init();
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

GLFWwindow* Window::getHandle() const { return handle; }

void Window::init()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw std::runtime_error("GLFW not Initialized Correctly.");
    }

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
