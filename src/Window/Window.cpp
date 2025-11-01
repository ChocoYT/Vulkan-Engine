#include "Window/Window.hpp"

#include <stdexcept>

Window::Window(
    GLFWwindow *handle,
    int width,
    int height,
    const std::string &name
) : handle(handle),
    width(width),
    height(height),
    name(&name)
{}

Window::~Window() = default;

std::unique_ptr<Window> Window::Create(
    int width,
    int height,
    const std::string &name
) {
    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("GLFW not Initialized Correctly.");

    // Create GLFW Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *handle = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!handle)
    {
        glfwTerminate();
        throw std::runtime_error("Window not Initialized Correctly.");
    }

    return std::unique_ptr<Window>(new Window(handle, width, height, name));
}