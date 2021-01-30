#include "glpp/context.hpp"

#include <cassert>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

gl::Context::Context(std::shared_ptr<gl::Context> shared) :
	mSharedContext(shared)
{
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
}

gl::GLFWContext::operator GLFWwindow* () const
{
	return mWindow;
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
}
