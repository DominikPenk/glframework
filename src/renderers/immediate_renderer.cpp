#include <glpp/renderers/immediate_renderer.hpp>

#include <glpp/camera.hpp>
#include <glpp/context.hpp>
#include <glpp/controls.hpp>
#include <glpp/framebuffer.hpp>
#include <glpp/intermediate.h>
#include <glpp/meshes.hpp>

#include <glpp/imgui.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glpp/imgui3d/imgui_3d.h>

#include <GLFW/glfw3.h>

gl::ImmediateRenderer::ImmediateRenderer() :
	mContext(nullptr),
	mImGui3DContext(nullptr),
	mOldImGui3DContext(nullptr),
	mFrameBuffer(nullptr),
	viewportCamera(std::make_shared<gl::Camera>()),
	viewportControl(std::make_shared<gl::FlyingControl>()),
	clearColor(0, 0, 0, 1)
{
}

gl::ImmediateRenderer::ImmediateRenderer(int width, int height, const std::string& title, std::shared_ptr<Context> shared) :
	ImmediateRenderer()
{
	mContext = std::make_shared<GLFWContext>(width, height, title, false, 4, 3, shared);
	initialize();
}

gl::ImmediateRenderer::ImmediateRenderer(const std::string& title, std::shared_ptr<Context> shared) :
	ImmediateRenderer()
{
	mContext = std::make_shared<GLFWContext>(1024, 720, title, true, 4, 3, shared);
	initialize();
}

bool gl::ImmediateRenderer::startFrame()
{
	mContext->makeCurrent();
	glfwPollEvents();

	if (mContext->isMinified()) { return false; }
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Update camera
	auto [w, h] = mContext->getWindowSize();
	viewportCamera->ScreenWidth = w;
	viewportCamera->ScreenHeight = h;
	mImGui3DContext->ScreenSize.x = w;
	mImGui3DContext->ScreenSize.y = h;
	mImGui3DContext->Aspect = (float)w / h;
	if (mFrameBuffer->width() != w || mFrameBuffer->height() != h) {
		mFrameBuffer->resize(w, h);
	}
	viewportControl->update(viewportCamera);

	mOldImGui3DContext = ImGui3D::GImGui3D;
	ImGui3D::SetContext(mImGui3DContext);
	ImGui3D::NewFrame(viewportCamera->viewMatrix, viewportCamera->GetProjectionMatrix());

	mFrameBuffer->bind();


	glViewport(0, 0, (int)w, (int)h);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mFrameBuffer->clearColorAttachment(0, clearColor);
	mFrameBuffer->clearColorAttachment(1, glm::vec4(0, 0, 0, 1));

}

void gl::ImmediateRenderer::endFrame()
{
	ImGui3D::Render();

	mFrameBuffer->unbind();
	auto [w, h] = mContext->getWindowSize();
	gl::displayTexture(0, 0, w, h, mFrameBuffer->getRenderTexture(0));

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(*mContext);
}

bool gl::ImmediateRenderer::shouldClose() const
{
	return mContext->shouldClose();
}

void gl::ImmediateRenderer::renderObjects(std::shared_ptr<Camera>) const
{
	glEnable(GL_DEPTH_TEST);
	for (auto obj : objects) {
		if (obj->visible) {
			obj->render(viewportCamera);
			obj->handleIO(viewportCamera, ImGui::GetIO());
		}
	}
}

size_t gl::ImmediateRenderer::addObject(std::shared_ptr<Mesh> mesh)
{
	objects.push_back(mesh);
	return objects.size() - 1;
}

void gl::ImmediateRenderer::initialize()
{
	// Initialize Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();		// FIXME: Nicer style
	ImGui_ImplGlfw_InitForOpenGL(*mContext, true);
	ImGui_ImplOpenGL3_Init(NULL);

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// add icons
	// See https://github.com/juliettef/IconFontCppHeaders#example-code
	// ---------------
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	std::string fontFile = std::string(GL_FRAMEWORK_FONT_DIR) + FONT_ICON_FILE_NAME_FAS;
	io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 13.0f, &icons_config, icons_ranges);
	
	// Init Framebuffer and ImGui3D
	auto [w, h] = mContext->getWindowSize();
	mFrameBuffer = std::make_shared<gl::Framebuffer>(w, h);
	mFrameBuffer->setRenderTexture(0, nullptr);
	mFrameBuffer->appendRenderTexture(nullptr);
	mImGui3DContext = ImGui3D::CreateContext();
	mImGui3DContext->GetHoveredIdImpl = [&](ImVec2 mouse) -> glm::uvec4 {
		return mFrameBuffer->readColorPixel(mouse.x, mouse.y, 1);
	};
}
