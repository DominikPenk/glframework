#pragma once

#include <array>
#include <memory>
#include <string>

struct GLFWwindow;

namespace gl {

	class Context {
	public:
		Context(std::shared_ptr<gl::Context> shared = nullptr);
		virtual void makeCurrent();

		static gl::Context* GetCurrentContext();

	protected:
		std::shared_ptr<gl::Context> mSharedContext;

	private:
		static gl::Context* sCurrentContext;
	};

	class GLFWContext : public gl::Context {
	public:
		GLFWContext(
			int width, int height,
			const std::string& title,
			bool maximized = false,
			int majorVersion = 4, 
			int minorVersion = 3, std::shared_ptr<gl::Context> shared = nullptr);
		~GLFWContext();

		virtual void makeCurrent();

		operator GLFWwindow* () const;

		void setFullscreenWindow(int monitorId) const;

		// New setters
		void setTitle(const std::string& title);
		void setWindowSize(int width, int height);
		void setCursorVisible(bool visible);


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

	class OffscreenContext : public gl::Context {
	public:
		OffscreenContext(int width, int height, std::shared_ptr<gl::Context> shared = nullptr);
		~OffscreenContext();

		virtual void makeCurrent() override;
	protected:
#ifdef WITH_EGL
		void* mDisplay;
		void* mConfig;
		void* mSurface;
		void* mContext;
#else
		GLFWwindow* mContext;
#endif
	};
}