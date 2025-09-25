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

        GLFWwindow* getHandle() const;

        int width;
        int height;
        std::string name;

        GLFWwindow *handle;

    private:
        void init();
};

#endif
