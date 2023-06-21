//
// Created by Shawwy on 6/18/2023.
//

#include "WindowSurface.h"

namespace IP::Wrapper {

    WindowSurface::WindowSurface(Instance::Ptr instance, Window::Ptr window) {
        mInstance = instance;
        if (glfwCreateWindowSurface(instance->getInstance(), window->getWindow(), nullptr, &mSurface) != VK_SUCCESS) {
            throw std::runtime_error("Error: failed to create surface");
        }
    }

    WindowSurface::~WindowSurface() {
        vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
        mInstance.reset();
    }
} // IP