#include <glpp/renderers/editor.hpp>
#include <glpp/renderers/editorwindow.hpp>

#include <glpp/camera.hpp>
#include <glpp/context.hpp>
#include <glpp/controls.hpp>
#include <glpp/framebuffer.hpp>
#include <glpp/imgui.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glpp/imgui3d/imgui_3d.h>
#include <glpp/intermediate.h>
#include <glpp/meshes.hpp>
#include <glpp/shadermanager.hpp>

#include <GLFW/glfw3.h>

gl::Editor::Editor() :
	mContext(nullptr),
	showOutliner(true),
	showDebug(true),
	gammaCorrection(true),
	toneMapping(ToneMapping::Reinhard),
	gamma(1.2f),
	clearColor(0.24f, 0.24f, 0.24f, 1.0f),
	mForceUiResetOnNextDraw(false)
{
	
}

gl::Editor::Editor(int width, int height, const std::string& title, EditorFlags flags) :
	Editor()
{
	mContext = std::make_shared<GLFWContext>(width, height, title);
	initialize(flags);
}

gl::Editor::Editor(const std::string& title, EditorFlags flags) :
	Editor()
{
	mContext = std::make_shared<GLFWContext>(1024, 720, title, true);
	initialize(flags);
}

bool gl::Editor::startFrame()
{
	mContext->makeCurrent();
	glfwPollEvents();

	if (mContext->isMinified()) { return false; }
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiID dockSpaceID = buildDefaultLayout(mForceUiResetOnNextDraw);
	mForceUiResetOnNextDraw = false;

	// Create central dockspace
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	ImGui::Begin("Central Widget", NULL, window_flags);
	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dockSpaceID, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);	// TODO: Put viewport into seperate window
	ImGui::End();
	
	bool isFirstDebug    = true;
	bool isFirstOutliner = true;
	for (auto window : mEditorWindows) {
		if (!showOutliner && window->is<OutlinerEditorWindow>() && isFirstOutliner) {
			isFirstOutliner = false;
			continue;
		}
		if (!showDebug && window->is<DebugEditorWindow>() && isFirstDebug) {
			isFirstDebug = false;
			continue;
		}
		window->preDraw(this);
		window->draw(this);
	}

	return true;
}

void gl::Editor::endFrame()
{
	mContext->makeCurrent();
	if (mContext->isMinified()) { return; }

	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(*mContext);
}

void gl::Editor::run()
{
	while (!mContext->shouldClose()) {
		startFrame();
		endFrame();
	}
}

bool gl::Editor::shouldClose() const
{
	return mContext->shouldClose();
}

size_t gl::Editor::addObject(const std::string& name, std::shared_ptr<Mesh> mesh)
{
	mObjects.push_back(mesh);
	mesh->name = name;
	return mObjects.size() - 1;
}

const std::vector<std::shared_ptr<gl::Mesh>>& gl::Editor::getObjects() const
{
	return mObjects;
}

std::vector<std::shared_ptr<gl::Mesh>> gl::Editor::getObjects()
{
	return mObjects;
}

std::shared_ptr<gl::ViewportEditorWindow> gl::Editor::getViewport(int id) const
{
	int viewportsFound = 0;
	for (std::shared_ptr<gl::EditorWindow> window : mEditorWindows) {
		if (std::dynamic_pointer_cast<gl::ViewportEditorWindow>(window) != nullptr) {
			viewportsFound += 1;
			if (viewportsFound > id) {
				return std::dynamic_pointer_cast<gl::ViewportEditorWindow>(window);
			}
		}
	}
	return nullptr;
}

std::shared_ptr<gl::Camera> gl::Editor::getViewportCamera(int id) const
{
	std::shared_ptr<gl::ViewportEditorWindow> window = getViewport(id);
	if (window == nullptr) { return nullptr; }
	return window->camera;
}

std::shared_ptr<gl::Control> gl::Editor::getViewportControls(int id) const
{
	std::shared_ptr<gl::ViewportEditorWindow> window = getViewport(id);
	if (window == nullptr) { return nullptr; }
	return window->controls;
}

void gl::Editor::resetUILayout()
{
	mForceUiResetOnNextDraw = true;
}

ImGuiID gl::Editor::buildDefaultLayout(bool force)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockSpaceID = ImGui::GetID("Dockspace");
	if (force && ImGui::DockBuilderGetNode(dockSpaceID) != NULL) {
		ImGui::DockBuilderRemoveNode(dockSpaceID);
	}
	if (ImGui::DockBuilderGetNode(dockSpaceID) == NULL) {
		ImGui::DockBuilderAddNode(dockSpaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockSpaceID, viewport->GetWorkSize());
		ImGuiID dockCenter = dockSpaceID;

		// Check which splits we need
		//bool needLeft   = std::any_of(mEditorWindows.begin(), mEditorWindows.end(), [](const auto& window) { return window->defaultRegion == EditorWindowRegion::Center; });
		//bool needRight  = std::any_of(mEditorWindows.begin(), mEditorWindows.end(), [](const auto& window) { return window->defaultRegion == EditorWindowRegion::Right; });
		//bool needTop    = std::any_of(mEditorWindows.begin(), mEditorWindows.end(), [](const auto& window) { return window->defaultRegion == EditorWindowRegion::Top; });
		//bool needBottom = std::any_of(mEditorWindows.begin(), mEditorWindows.end(), [](const auto& window) { return window->defaultRegion == EditorWindowRegion::Bottom; });

		ImGuiID dockRight  = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.25, NULL, &dockCenter);
		ImGuiID dockLeft   = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left,  0.25, NULL, &dockCenter);
		ImGuiID dockTop    = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Up,    0.05, NULL, &dockCenter);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down,  0.15, NULL, &dockCenter);

		// Dock the windows
		std::unordered_map<EditorWindowRegion, ImGuiID> regions;
		regions[EditorWindowRegion::Center] = dockCenter;
		regions[EditorWindowRegion::Left]   = dockLeft;
		regions[EditorWindowRegion::Right]  = dockRight;
		regions[EditorWindowRegion::Bottom] = dockBottom;
		regions[EditorWindowRegion::Top]    = dockTop;
		for (const auto& window : mEditorWindows) {
			if (window->defaultRegion == EditorWindowRegion::Floating) { continue; }
			ImGui::DockBuilderDockWindow(window->title.c_str(), regions[window->defaultRegion]);
		}

		ImGui::DockBuilderFinish(dockSpaceID);

	}

	return dockSpaceID;
}

void gl::Editor::initialize(EditorFlags flags)
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

	// Add the default windows "Debug" and "Outliner"
	if ((flags & EditorFlags_NoDefaultOutliner) == 0x0) {
		mEditorWindows.push_back(std::make_shared<OutlinerEditorWindow>());
	}
	if ((flags & EditorFlags_NoDefaultDebug) == 0x0) {
		mEditorWindows.push_back(std::make_shared<DebugEditorWindow>());
	}
	if ((flags & EditorFlags_NoDefaultViewport) == 0x0) {
		mEditorWindows.push_back(std::make_shared<ViewportEditorWindow>());
	}
	if ((flags & EditorFlags_NoDefaultLogging) == 0x0) {
		mEditorWindows.push_back(std::make_shared<LoggingEditorWindow>());
	}
}
