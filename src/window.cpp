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
    glfwDestroyWindow(window);
    glfwTerminate();
}

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

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Window not Initialized Correctly.");
    }

    // Get Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "[INFO] " << extensionCount << " extensions supported." << std::endl;
}
