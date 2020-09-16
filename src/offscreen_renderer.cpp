#include "offscreen_renderer.hpp"

gl::OffscreenRenderer::OffscreenRenderer(std::shared_ptr<Camera> cam) :
	gl::RendererBase(cam)
{
	if (glfwInit() == 0) throw std::runtime_error("Failed to init GLFW");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, mVersion[0]);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, mVersion[1]);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(1, 1, "Offscreen Window", NULL, NULL);
	if (mWindow == NULL)
	{
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(mWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glfwMakeContextCurrent(mWindow);
	glfwSetWindowUserPointer(mWindow, this);
	//glfwSetWindowSize(mWindow, mCamera->ScreenWidth, mCamera->ScreenHeight);

	// set callbacks
	//glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

#if _DEBUG
	if (glDebugMessageCallback) {
		std::cout << "Register OpenGL debug callback " << std::endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(
			GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
	}
	else
		std::cout << "glDebugMessageCallback not available" << std::endl;
#endif

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
