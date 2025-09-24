#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#ifndef WINDOW_HPP
#define WINDOW_HPP

class Window
{
    public:
        Window(int width, int height, std::string name);
        ~Window();

        int width;
        int height;
        std::string name;

        GLFWwindow *window;

    private:
        void init();
};

#endif
