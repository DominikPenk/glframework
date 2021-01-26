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
