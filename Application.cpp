//
// Created by Shawwy on 6/13/2023.
//

#include "Application.h"

namespace IP
{
    void Application::run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

    void Application::initWindow()
    {
        glfwInit();

        //setting up environment, inactivate opengl API
        //and ban any changes on window's size
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(WIDTH,HEIGHT,"IndividualProject", nullptr, nullptr);
        if (!mWindow) {
            std::cerr << "Error: failed to create window;" << std::endl;
        }else
        {
            std::cout << "Successfully create the window " << std::endl;
        }
    }

    void Application::initVulkan() {
        mInstance = Wrapper::vInstance::create(true);
    }

    void Application::mainLoop() {
        while(!glfwWindowShouldClose(mWindow))
        {
            glfwPollEvents();
        }
    }

    void Application::cleanUp() {
        mInstance.reset();
        glfwDestroyWindow(mWindow);
        glfwTerminate();
        std::cout << "Destroy the window" << std::endl;
    }
}