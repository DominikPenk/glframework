#include "glpp/offscreen_renderer.hpp"

gl::OffscreenRenderer::OffscreenRenderer(std::shared_ptr<Camera> cam) :
	gl::RendererBase(1, 1, "Offscreen Window", false),
	mCamera(cam)
{
	// we enable depth
	// ---------------
	glEnable(GL_DEPTH_TEST);
}

gl::OffscreenRenderer::~OffscreenRenderer()
{
	if (NULL != mWindow) glfwDestroyWindow(mWindow);
	mWindow = NULL;
}

void gl::OffscreenRenderer::startRender(size_t width, size_t height)
{
	glfwMakeContextCurrent(mWindow);
	mCamera->ScreenWidth = width;
	mCamera->ScreenHeight = height;
	glViewport(0, 0, width, height);
	
}

bool gl::OffscreenRenderer::startFrame()
{
	throw std::runtime_error("You should not call `startFrame` on OffscreenRenderer");
}

void gl::OffscreenRenderer::endFrame()
{
	throw std::runtime_error("You should not call `endFrame` on OffscreenRenderer");
}
