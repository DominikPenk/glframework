#pragma once

#include <array>
#include <memory>
#include <string>



struct GLFWwindow;

namespace gl {

	class Context {
	public:
		Context(std::shared_ptr<gl::Context> shared = nullptr);
		virtual void makeCurrent() = 0;

	protected:
		std::shared_ptr<gl::Context> mSharedContext;
	};

	class GLFWContext : Context {
	public:
		GLFWContext(
			int width, int height,
			const std::string& title = "Window Title",
			bool maximized = false,
			int majorVersion = 4, 
			int minorVersion = 3, std::shared_ptr<gl::Context> shared = nullptr);

		virtual void makeCurrent();

		operator GLFWwindow* () const;

		// New setters
		void setTitle(const std::string& title);
		void setWindowSize(int width, int height);

		bool shouldClose() const;
		bool isMinified() const;
		bool isMaximized() const;

		// New getters
		std::pair<int, int> getWindowSize() const;
		int                 getWindowWidth() const;
		int                 getWindowHeight() const;
		std::string         getTitle() const;

		const std::array<int, 2> version;

	protected:
		std::string mTitle;
		GLFWwindow* mWindow;
	};

}