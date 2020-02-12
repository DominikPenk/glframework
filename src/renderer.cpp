#include "renderer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "imgui3d/imgui_3d.h"

#include "IconsFontAwesome5.h"
#include "uiwindow.hpp"

using namespace gl;

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {

	// Some filters
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

	std::cout << "---------------------opengl-Error------------" << std::endl;
	std::cout << "message: " << message << std::endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "NOTIFICATION";
		break;
	default:
		std::cout << "UNKNOWN";
	}
	std::cout << std::endl;
	std::cout << "---------------------opengl-Error-end--------------" << std::endl;
#ifdef _MSC_VER 
	if(severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR)
		__debugbreak();
#endif
}

// glfw: whenever the window is resized
// -------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* renderer = ((Renderer*)glfwGetWindowUserPointer(window));
	renderer->camera()->ScreenWidth = width;
	renderer->camera()->ScreenHeight = height;
	for (auto fn : renderer->resizeCallbacks()) {
		fn(renderer);
	}

	// Update screen size for ImGui3D
	if (nullptr != ImGui3D::GImGui3D) {
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		g.ScreenSize.x = static_cast<float>(width);
		g.ScreenSize.y = static_cast<float>(height);
		g.Aspect = width / (float)height;
	}

	// Resize frame buffer
	renderer->framebuffer()->resize(width, height);
	glViewport(0, 0, width, height);
}

Renderer::Renderer(int width, int height, std::shared_ptr<Camera> cam, const std::string& title, bool maximized) :
	mCamera(cam),
	mOutliner(std::make_unique<OutlinerWindow>()),
	mDebugWindow(std::make_unique<RendererDebugWindow>()),
	showOutliner(true),
	showDebug(false),
	gammaCorrection(true),
	mToneMapping(ToneMapping::Reinhard),
	gamma(1.2f),
	mPostProShader(std::string(GL_FRAMEWORK_SHADER_DIR) + "displayShader.glsl"),
	mDisplayShader(std::string(GL_FRAMEWORK_SHADER_DIR) + "copyShader.glsl")
{

	mWindow = NULL;
	mCamera->ScreenWidth = width;
	mCamera->ScreenHeight = height;
	m_version[0] = 4;
	m_version[1] = 3;
	mClearColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

	// Create window to get a context
	if (glfwInit() == 0) throw std::runtime_error("Failed to init GLFW");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);

	mWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
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

	if (maximized) {
		glfwGetWindowSize(mWindow, &mCamera->ScreenWidth, &mCamera->ScreenHeight);
	}
	else {
		glfwSetWindowSize(mWindow, mCamera->ScreenWidth, mCamera->ScreenHeight);
	} 

	// set callbacks
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

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

	// Create the frame buffer for rendering
	mFrameBuffer = std::make_shared<gl::Framebuffer>(mCamera->ScreenWidth, mCamera->ScreenHeight);
	mFrameBuffer->setRenderTexture(0, nullptr);
	mFrameBuffer->appendRenderTexture(nullptr);

	// enable imgui
	// ---------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init(NULL);
	ImGui3d_ImplRenderer_Init(ImGui3D::CreateContext());

	// add icons
	// See https://github.com/juliettef/IconFontCppHeaders#example-code
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	std::string fontFile = std::string(GL_FRAMEWORK_FONT_DIR) + FONT_ICON_FILE_NAME_FAS;
	io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 13.0f, &icons_config, icons_ranges);

	// Add callback to imgui3d impl
	ImGui3D::GImGui3D->GetHoveredIdImpl = [&](ImVec2 mouse) -> glm::uvec4 {
		return mFrameBuffer->readColorPixel((int)mouse.x, (int)mouse.y, 1);
	};

	setToneMapping(mToneMapping);
}

Renderer::~Renderer() {
	if (NULL != mWindow) glfwTerminate();
	mWindow = NULL;
}

bool gl::Renderer::startFrame()
{
	glfwMakeContextCurrent(mWindow);
	glfwPollEvents();

	// Skip if minimized
	if (glfwGetWindowAttrib(mWindow, GLFW_ICONIFIED))
		return false;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui3D::NewFrame(mCamera->viewMatrix, mCamera->GetProjectionMatrix());

	// initialize a new imgui frame
	if (mUIWindows.size() > 0) {
		for (auto window : mUIWindows) {
			window->preDraw(this);
			window->draw(this);
		}
	}
	if (showOutliner) {
		mOutliner->draw(this);
	}

	if (showDebug) {
		mDebugWindow->draw(this);
	}

	return true;
}

void gl::Renderer::endFrame()
{
	// Skip if minimized
	if (glfwGetWindowAttrib(mWindow, GLFW_ICONIFIED))
		return;

	mFrameBuffer->bind();
	glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.w);
	mFrameBuffer->clear({ mClearColor, glm::vec4(0, 0, 0, 1) });

	for (auto mesh : mMeshes) {
		if (mesh->visible) {
			mesh->render(this);
			mesh->handleIO(this, ImGui::GetIO());
		}

	}
	
	// Do post processing
	if (mToneMapping != ToneMapping::Linear || gammaCorrection) {
		glDisable(GL_DEPTH_TEST);
		mPostProShader.use();
		mPostProShader.setUniform("gamma", gammaCorrection ? gamma : 1.0f);
		mPostProShader.setUniform("hdr", hdr);
		mFrameBuffer->getRenderTexture(0)->bind(0);
		mDummyVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		mDummyVAO.unbind();
		glEnable(GL_DEPTH_TEST);
	}

	mFrameBuffer->clearColorAttachment(1, glm::vec4(0, 0, 0, 1));
	// Render all ui windows that draw into viewport
	if (mUIWindows.size() > 0) {
		for (auto window : mUIWindows) {
			window->viewportDraw(this);
		}
	}

	for (auto mesh : mMeshes) {
		if (mesh->visible) {
			mesh->drawViewportUI(this);
		}
	}
	// Render ImGui3D
	ImGui3D::Render();
	mFrameBuffer->unbind();

	// Draw Result to screen
	glDisable(GL_DEPTH_TEST);
	mDisplayShader.use();
	mFrameBuffer->getRenderTexture(0)->bind(0);
	mDummyVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	mDummyVAO.unbind();
	glEnable(GL_DEPTH_TEST);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	glfwSwapBuffers(mWindow);
}

size_t gl::Renderer::addMesh(const std::string& name, std::shared_ptr<Mesh> mesh)
{
	mMeshes.push_back(mesh);
	mesh->name = name;
	return mMeshes.size() - 1;
}

void gl::Renderer::setTitle(const std::string& title)
{
	glfwSetWindowTitle(mWindow, title.c_str());
}

const std::vector<std::shared_ptr<Mesh>>& gl::Renderer::objects() const
{
	return mMeshes;
}

std::vector<std::shared_ptr<Mesh>>& gl::Renderer::objects()
{
	return mMeshes;
}

void gl::Renderer::setToneMapping(ToneMapping mapping)
{
	mToneMapping = mapping;
	mPostProShader.setDefine("HDR_MAPPING_TYPE", (int)mToneMapping);
	mPostProShader.update();
}

void gl::Renderer::pushResizeCallback(std::function<void(Renderer*)> fn)
{
	mResizeCallbacks.push_back(fn);
}

std::vector<std::function<void(Renderer*)>> gl::Renderer::resizeCallbacks()
{
	return mResizeCallbacks;
}

void gl::Renderer::ImGui3d_ImplRenderer_Init(std::shared_ptr<ImGui3D::ImGui3DContext> ctx)
{
	ctx->ScreenSize = ImVec2((float)mCamera->ScreenWidth, (float)mCamera->ScreenHeight);
	ctx->Aspect = mCamera->ScreenWidth / (float)mCamera->ScreenHeight;
	//ctx->IdTexture = mFrameBuffer->getRenderTexture(1);
}

void gl::Renderer::addUIWindow(std::shared_ptr<UIWindow> window)
{
	mUIWindows.push_back(window);
}

std::shared_ptr<GenericUIWindow> gl::Renderer::addUIWindow(std::string title, std::function<void(Renderer*)> drawFn)
{
	return addUIWindow<GenericUIWindow>(title, drawFn);
}
