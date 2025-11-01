#pragma once

#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#include "Settings.hpp"

class Window
{
    public:
        ~Window();
        
        static std::unique_ptr<Window> Create(
            int width  = SCREEN_WIDTH,
            int height = SCREEN_HEIGHT,
            const std::string &name = SCREEN_NAME
        );

        // Getters
        int GetWidth()  const { return width; }
        int GetHeight() const { return height; }
        const std::string& GetName()   const { return *name; }

        GLFWwindow* GetHandle() const { return handle; }

    private:
        Window() = default;
        Window(
            GLFWwindow *handle,
            int width,
            int height,
            const std::string &name
        );

        GLFWwindow *handle = nullptr;

        int width;
        int height;
        const std::string* name = nullptr;
};
