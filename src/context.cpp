#include "glpp/context.hpp"

#include <cassert>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

gl::Context* gl::Context::sCurrentContext = nullptr;

gl::Context::Context(std::shared_ptr<gl::Context> shared) :
	mSharedContext(shared)
{
}

void gl::Context::makeCurrent() {
	sCurrentContext = this;
}

gl::Context* gl::Context::GetCurrentContext()
{
	return sCurrentContext;
}

gl::GLFWContext::GLFWContext(
	int width, int height,
	const std::string& title,
	bool maximized,
	int major,
	int minor, 
	std::shared_ptr<gl::Context> shared) :
	Context(shared),
	mWindow(nullptr),
	mTitle(title),
	version({major, minor})
{
	if (glfwInit() == 0) { throw std::runtime_error("Failed to initialize GLFW"); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);

	GLFWwindow *sharedContext = nullptr;
	if (shared != nullptr && std::dynamic_pointer_cast<gl::GLFWContext>(shared) != nullptr) {
		sharedContext = std::dynamic_pointer_cast<gl::GLFWContext>(shared)->mWindow;
	}

	mWindow = glfwCreateWindow(width, height, title.c_str(), NULL, sharedContext);
	if (mWindow == nullptr)
	{
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(mWindow);

	// Load opengl functions
	// ---------------
	if (!gladLoadGL())
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}
}

gl::GLFWContext::~GLFWContext()
{
	if (mWindow != nullptr) {
		glfwDestroyWindow(mWindow);
	}
}

void gl::GLFWContext::makeCurrent()
{
	assert(mWindow != nullptr);
	glfwMakeContextCurrent(mWindow);
	gl::Context::makeCurrent();
}

gl::GLFWContext::operator GLFWwindow* () const
{
	return mWindow;
}

void gl::GLFWContext::setFullscreenWindow(int monitorId) const
{
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	if (count <= monitorId) {
		throw std::runtime_error("Tried to create a fullscreen window for monitor id " + std::to_string(monitorId) + " but only found " + std::to_string(count) + " monitors");
	}
	const GLFWvidmode* mode = glfwGetVideoMode(monitors[monitorId]);
	glfwSetWindowAttrib(mWindow, GLFW_RED_BITS, mode->redBits);
	glfwSetWindowAttrib(mWindow, GLFW_GREEN_BITS, mode->greenBits);
	glfwSetWindowAttrib(mWindow, GLFW_BLUE_BITS, mode->blueBits);
	glfwSetWindowAttrib(mWindow, GLFW_AUTO_ICONIFY, GL_FALSE);
	int x, y;
	glfwGetMonitorPos(monitors[monitorId], &x, &y);
	glfwSetWindowMonitor(mWindow, monitors[monitorId], x, y, mode->width, mode->height, mode->refreshRate);
}

void gl::GLFWContext::setTitle(const std::string& title)
{
	assert(mWindow != nullptr);
	glfwSetWindowTitle(mWindow, title.c_str());
	mTitle = title;
}

void gl::GLFWContext::setWindowSize(int width, int height)
{
	assert(mWindow != nullptr);
	glfwSetWindowSize(mWindow, width, height);
}

void gl::GLFWContext::setCursorVisible(bool visible)
{
	glfwSetInputMode(mWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

bool gl::GLFWContext::shouldClose() const
{
	assert(mWindow != nullptr);
	return glfwWindowShouldClose(mWindow);
}

bool gl::GLFWContext::isMinified() const
{
	assert(mWindow != nullptr);
	return glfwGetWindowAttrib(mWindow, GLFW_ICONIFIED);
}

bool gl::GLFWContext::isMaximized() const
{
	assert(mWindow != nullptr);
	return glfwGetWindowAttrib(mWindow, GLFW_MAXIMIZED);
}

std::pair<int, int> gl::GLFWContext::getWindowSize() const
{
	assert(mWindow != nullptr);
	std::pair<int, int> result;
	glfwGetWindowSize(mWindow, &result.first, &result.second);
	return result;
}

int gl::GLFWContext::getWindowWidth() const
{
	auto [w, h] = getWindowSize();
	return w;
}

int gl::GLFWContext::getWindowHeight() const
{
	auto [w, h] = getWindowSize();
	return h;
}

std::string gl::GLFWContext::getTitle() const
{
	return mTitle;
}

gl::OffscreenContext::OffscreenContext(int width, int height, std::shared_ptr<gl::Context> shared) :
	Context(shared),
#ifdef WITH_EGL
	mDisplay(nullptr),
	mConfig(nullptr),
	mSurface(nullptr)
#endif
	mContext(nullptr)
{
#ifdef WITH_EGL
	// 1. Initialize EGL
	mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	EGLint major, minor;
	eglInitialize(mDisplay, &major, &minor);

	// 2. Select an appropriate configuration
	EGLint numConfigs;
	EGLint configAttribs[] = {
		EGL_SURFACE_TYPE,    EGL_PBUFFER_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,  EGL_NONE };
	eglChooseConfig(mDisplay, configAttribs, &mConfig, 1, &numConfigs);

	// 3. Create a surface
	EGLint pbufferAttribs[] = {
		EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE,
	};
	mSurface = eglCreatePbufferSurface(mDisplay, mConfig, pbufferAttribs);

	// 4. Bind the API
	eglBindAPI(EGL_OPENGL_API);

	// 5. Create a context and make it current
	mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, NULL);
#else
	if (glfwInit() == 0) throw std::runtime_error("Failed to init GLFW");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	mContext = glfwCreateWindow(width, height, "Render Context", NULL, NULL);
	if (mContext == NULL) {
		throw std::runtime_error("Failed to create context");
	}
#endif

	makeCurrent();

	// Load opengl functions
	if (!gladLoadGL())
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

}

gl::OffscreenContext::~OffscreenContext()
{
#ifdef WITH_EGL
	if (mDisplay != nullptr) {
		eglTerminate(mDisplay);
	}
#else
	if (mContext != nullptr) {
		glfwDestroyWindow(mContext);
	}
#endif
}

void gl::OffscreenContext::makeCurrent()
{
#ifdef WITH_EGL
	eglMakeCurrent(mDissplay, mSurface, mSurface, mContext);
#else
	glfwMakeContextCurrent(mContext);
#endif
	gl::Context::makeCurrent();
}
