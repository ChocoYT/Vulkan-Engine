#pragma once

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class Window
{
    public:
        Window(int width, int height, std::string name);
        ~Window();

        GLFWwindow* getHandle() const;

        int width;
        int height;
        std::string name;

        GLFWwindow *handle;

    private:
        void init();
};
