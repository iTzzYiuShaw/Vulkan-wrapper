//
// Created by Shawwy on 6/13/2023.
//
#pragma once
#ifndef INDIVIDUALPROJECT_APPLICATION_H
#define INDIVIDUALPROJECT_APPLICATION_H

#include "base.h"
#include "vulkanWrapper/instance.h"
#include "vulkanWrapper/device.h"
namespace IP
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    class Application {

    public:
        Application() = default;
        ~Application() = default;

        void run();

    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void cleanUp();

    private:
        GLFWwindow* mWindow{NULL};
        Wrapper::instance::Ptr mInstance {nullptr};
        Wrapper::device::Ptr mDevice {nullptr};
    };
}



#endif //INDIVIDUALPROJECT_APPLICATION_H
