//
// Created by Shawwy on 6/18/2023.
//
#include "../base.h"

#ifndef INDIVIDUALPROJECT_WINDOW_H
#define INDIVIDUALPROJECT_WINDOW_H

namespace IP::Wrapper {

    class Window {
    public:
        using Ptr = std::shared_ptr<Window>;
        static Ptr create(const int& width, const int& height) {
            return std::make_shared<Window>(width, height);
        }

        //Initialize the window
        Window(const int &width, const int &height);

        ~Window();

        //Check if the window should be closed
        bool shouldClose();

        void pollEvents();

        [[nodiscard]] auto getWindow() const { return mWindow; }

    public:
        bool mWindowResized{ false };
    private:
        int mWidth{ 0 };
        int mHeight{ 0 };
        GLFWwindow* mWindow{ NULL };
    };

} // Wrapper

#endif //INDIVIDUALPROJECT_WINDOW_H
