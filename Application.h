//
// Created by Shawwy on 6/13/2023.
//
#pragma once
#ifndef INDIVIDUALPROJECT_APPLICATION_H
#define INDIVIDUALPROJECT_APPLICATION_H

#include "base.h"
#include "vulkanWrapper/Instance.h"
#include "vulkanWrapper/Device.h"
#include "vulkanWrapper/Window.h"
#include "vulkanWrapper/WindowSurface.h"
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
        Wrapper::Window::Ptr mWindow {nullptr};
        Wrapper::Instance::Ptr mInstance {nullptr};
        Wrapper::Device::Ptr mDevice {nullptr};
        Wrapper::WindowSurface::Ptr mSurface{ nullptr };
    };
}



#endif //INDIVIDUALPROJECT_APPLICATION_H
