#pragma once

#include "../base.h"

namespace IP {
	class Application;
}

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

		void setApp(std::shared_ptr<Application> app) { mApp = app; }

		void processEvent();

	public:
		bool mWindowResized{ false };
		std::weak_ptr<Application> mApp;

	private:
		int mWidth{ 0 };
		int mHeight{ 0 };
		GLFWwindow* mWindow{ NULL };

	};
}
