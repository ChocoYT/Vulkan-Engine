#pragma once

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class Window
{
    public:
        void init(int width, int height, const std::string &name);
        void cleanup();

        // Getters
        int getWidth()  const { return width;  }
        int getHeight() const { return height; }
        const std::string& getName() const { return *name; }

        GLFWwindow* getHandle() const { return handle; }

    private:
        int width;
        int height;
        const std::string* name = nullptr;

        GLFWwindow *handle = nullptr;
};
