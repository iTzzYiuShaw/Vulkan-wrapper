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
        mWindow = Wrapper::Window::create(WIDTH,HEIGHT);
    }

    void Application::initVulkan() {
        mInstance = Wrapper::Instance::create(true);
        mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);
        mDevice = Wrapper::Device::create(mInstance);
    }

    void Application::mainLoop() {
        while(!mWindow->shouldClose())
        {
            mWindow->pollEvents();
        }
    }

    void Application::cleanUp() {
        mDevice.reset();
        mSurface.reset();
        mInstance.reset();
        mWindow.reset();
    }
}