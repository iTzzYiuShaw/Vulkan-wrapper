//
// Created by Shawwy on 6/18/2023.
//

#include "Window.h"

namespace IP::Wrapper {

    //Initialize glfw window
    Window::Window(const int& width, const int& height) {
        mWidth = width;
        mHeight = height;

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(mWidth, mHeight, "vulkan window", nullptr, nullptr);
        if (!mWindow) {
            std::cerr << "Error: failed to create window" << std::endl;
        }
    }

    Window::~Window() {
        glfwDestroyWindow(mWindow);
        glfwTerminate();
        std::cout << "Destroy the window" << std::endl;
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(mWindow);
    }

    void Window::pollEvents() {
        glfwPollEvents();
    }

} // Wrapper