//
// Created by Shawwy on 6/18/2023.
//

#include "../base.h"
#include "Window.h"
#include "Instance.h"
#ifndef INDIVIDUALPROJECT_WINDOWSURFACE_H
#define INDIVIDUALPROJECT_WINDOWSURFACE_H

namespace IP::Wrapper {

    class WindowSurface {

    public:
        using Ptr = std::shared_ptr<WindowSurface>;
        static Ptr create(Instance::Ptr instance, Window::Ptr window) {
            return std::make_shared<WindowSurface>(instance, window);
        }

        WindowSurface(Instance::Ptr instance, Window::Ptr window);

        ~WindowSurface();

        [[nodiscard]] auto getSurface() const { return mSurface; }

    private:
        VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
        Instance::Ptr mInstance{ nullptr };
    };
} // IP

#endif //INDIVIDUALPROJECT_WINDOWSURFACE_H
