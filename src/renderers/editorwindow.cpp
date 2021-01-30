#include <glpp/renderers/editorwindow.hpp>
#include <glpp/renderers/editor.hpp>

#include <glpp/imgui.hpp>
#include <glpp/imgui3d/imgui_3d.h>

#include <glpp/controls.hpp>
#include <glpp/framebuffer.hpp>
#include <glpp/intermediate.h>
#include <glpp/logging.hpp>
#include <glpp/meshes.hpp>
#include <glpp/shadermanager.hpp>

gl::EditorWindow::EditorWindow(const std::string& title, EditorWindowRegion defaultRegion) :
	title(title),
	defaultRegion(defaultRegion),
	open(true),
	initialized(false),
	position(-1, -1),
	size(0, 0)
{
}

void gl::EditorWindow::draw(Editor* editor)
{
	if (ImGui::Begin(title.c_str(), &open)) {
		ImVec2 windowPos  = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		if (!initialized) {
			position = windowPos;
			size = windowSize;
			initialize(editor);
		}
		// Check if resized
		if (!initialized || (int)windowPos.x != (int)position.x || (int)windowPos.y != (int)position.y || (int)windowSize.x != (int)size.x || (int)windowSize.y != (int)size.y) {
			position = windowPos;
			size     = windowSize;
			onResize(windowPos, windowSize, editor);
		}

		onDraw(editor);
		initialized = true;
	}
	ImGui::End();
}

void gl::EditorWindow::preDraw(Editor* editor)
{
}

void gl::EditorWindow::viewportDraw(Editor* editor)
{
}

void gl::EditorWindow::onResize(ImVec2 position, ImVec2 windowSize, Editor* editor)
{
}

void gl::EditorWindow::initialize(Editor* editor)
{
}

gl::OutlinerEditorWindow::OutlinerEditorWindow(const std::string& title, EditorWindowRegion defaultRegion) :
	EditorWindow(title, defaultRegion)
{
}

void gl::OutlinerEditorWindow::onDraw(Editor* editor)
{
	int idx = 0;
	for (auto obj : editor->getObjects()) {
		if (!obj->mShowInOutliner) continue;
		ImGui::PushID(idx++);
		if (ImGui::Button(obj->visible ? ICON_FA_EYE : ICON_FA_EYE_SLASH)) {
			obj->visible = !obj->visible;
		}
		ImGui::SameLine();
		ImGui::Text(obj->name.c_str());
		ImGui::Indent();
		obj->drawOutliner();
		ImGui::Unindent();
		ImGui::PopID();

	}
}

gl::DebugEditorWindow::DebugEditorWindow(const std::string& title, EditorWindowRegion defaultRegion) :
	EditorWindow(title, defaultRegion)
{
}

void gl::DebugEditorWindow::onDraw(Editor* editor)
{
	const char* hdrmappings[] = {
		"Linear",
		"Reinhard",
		"Haarm-Peter Duiker",
		"Jim Hejl, Richard Burgess-Dawson",
		"Uncharted 2"
	};

	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::TreeNodeEx("Post processing", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginCombo("HDR Mapping", hdrmappings[(int)editor->toneMapping])) {
			for (int i = 0; i < IM_ARRAYSIZE(hdrmappings); ++i) {
				bool isSelected = i == (int)editor->toneMapping;
				if (ImGui::Selectable(hdrmappings[i], isSelected)) {
					editor->toneMapping = static_cast<ToneMapping>(i);
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Checkbox("Gamma Correction", &editor->gammaCorrection);
		if (editor->gammaCorrection) {
			ImGui::DragFloat("Gamma", &editor->gamma, .1f, 0.1f, 4.0f);
		}
		ImGui::TreePop();
	}

	std::vector<std::shared_ptr<gl::ViewportEditorWindow>> viewports = editor->getEditorWindows<gl::ViewportEditorWindow>();
	for (size_t i = 0; i < viewports.size(); ++i) {
		std::string title = "Viewport " + std::to_string(i);
		auto frambuffer = viewports[i]->mFrameBuffer;
		if (ImGui::TreeNode(title.c_str())) {
			// Todo move this to ImGuiExtension
			auto AspectImage = [](GLuint tid, const int width, const int height, const ImVec2 maxSize, const ImVec2 uv0 = ImVec2(0, 0), const ImVec2 uv1 = ImVec2(1, 1), const ImVec4 tint_color = ImVec4(1, 1, 1, 1)) {
				float sx = (float)maxSize.x / (float)width;
				float sy = (float)maxSize.y / (float)height;
				float s = std::min(sx, std::min(sy, 1.0f));
				ImGui::Image((GLuint*)tid, ImVec2((float)width, (float)height) * s, uv0, uv1, tint_color);
			};

			std::shared_ptr<gl::Texture> colorTexture = frambuffer->getRenderTexture(0);
			std::shared_ptr<gl::Texture> idTexture = frambuffer->getRenderTexture(1);
			AspectImage(colorTexture->id, colorTexture->cols, colorTexture->rows, ImVec2(500, 500), ImVec2(0, 1), ImVec2(1, 0));
			AspectImage(idTexture->id, colorTexture->cols, colorTexture->rows, ImVec2(500, 500), ImVec2(0, 1), ImVec2(1, 0));

			// Get color under the cursor
			/*ImVec2 mouse = ImGui::GetIO().MousePos;
			glm::uvec4 color = frambuffer->readColorPixel((int)mouse.x, (int)mouse.y, 1);
			ImGuiID id = ImGui3D::ColorToID(color.r, color.g, color.b);

			ImGui::Text("Mouse at (%d, %d) over color [%d, %d, %d], Id: %d", (int)mouse.x, (int)mouse.y, color.x, color.y, color.z, id);

			{
				unsigned int id = 1615879;
				glm::vec3 idToCol = ImGui3D::IDToColor(id);
				ImGuiID test = ImGui3D::ColorToID(idToCol.r * 255, idToCol.g * 255, idToCol.b * 255);
				ImGui::Text("%d -> (%d, %d, %d) -> %d", id, (int)(idToCol.r * 255), (int)(idToCol.g * 255), (int)(idToCol.b * 255), test);
			}

			{
				unsigned int id = 11708569;
				glm::vec3 idToCol = ImGui3D::IDToColor(id);
				ImGuiID test = ImGui3D::ColorToID(idToCol.r * 255, idToCol.g * 255, idToCol.b * 255);
				ImGui::Text("%d -> (%d, %d, %d) -> %d", id, (int)(idToCol.r * 255), (int)(idToCol.g * 255), (int)(idToCol.b * 255), test);
			}

			{
				unsigned int id = 4042620;
				glm::vec3 idToCol = ImGui3D::IDToColor(id);
				ImGuiID test = ImGui3D::ColorToID(idToCol.r * 255, idToCol.g * 255, idToCol.b * 255);
				ImGui::Text("%d -> (%d, %d, %d) -> %d", id, (int)(idToCol.r * 255), (int)(idToCol.g * 255), (int)(idToCol.b * 255), test);
			}

			ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
			ImGui::Text("Active id: %d (color: %d, %d, %d)", g.ActiveId, g.ActiveIdColor.r, g.ActiveIdColor.g, g.ActiveIdColor.b);*/

			ImGui::TreePop();

		}
	}
}

gl::ViewportEditorWindow::ViewportEditorWindow(const std::string& title, EditorWindowRegion defaultRegion) :
	EditorWindow(title, defaultRegion),
	mFrameBuffer(nullptr),
	mGeometryFrameBuffer(nullptr),
	mLastTonemapping(ToneMapping::Reinhard)
{
}

void gl::ViewportEditorWindow::initialize(Editor* editor) {
	// Initialize framebuffers
	int w = (int)size.x;
	int h = (int)size.y;
	mGeometryFrameBuffer = std::make_shared<gl::Framebuffer>(w, h);
	mGeometryFrameBuffer->setRenderTexture(0, nullptr);
	auto depthTexture = mGeometryFrameBuffer->setDepthTexture(nullptr);
	mFrameBuffer = std::make_shared<gl::Framebuffer>(w, h);
	mFrameBuffer->setRenderTexture(0, nullptr);
	mFrameBuffer->appendRenderTexture(nullptr);
	mFrameBuffer->setDepthTexture(depthTexture);

	// Initialize postporcessing shader
	mTonemappingShader = std::make_unique<gl::Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "displayShader.glsl");
	mTonemappingShader->setDefine("HDR_MAPPING_TYPE", static_cast<int>(editor->toneMapping));
	mTonemappingShader->update();
	mLastTonemapping = editor->toneMapping;

	// Initialize ImGui3D
	mImGui3DContext = ImGui3D::CreateContext();
	mImGui3DContext->GetHoveredIdImpl = [&](ImVec2 mouse) -> glm::uvec4 {
		if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused()) {
			ImVec2 windowPos = ImGui::GetWindowPos();
			return mFrameBuffer->readColorPixel(mouse.x - windowPos.x, mouse.y - windowPos.y, 1);
		}
		else {
			return glm::uvec4(0, 0, 0, 255);
		}
	};
}

void gl::ViewportEditorWindow::onResize(ImVec2 position, ImVec2 size, Editor* editor) {
	editor->viewportCamera->ScreenWidth  = (int)size.x;
	editor->viewportCamera->ScreenHeight = (int)size.y;
	mImGui3DContext->ScreenSize.x = size.x;
	mImGui3DContext->ScreenSize.y = size.y;
	mImGui3DContext->Aspect = size.x / size.y;
	mFrameBuffer->resize((int)size.x, (int)size.y);
	mGeometryFrameBuffer->resize((int)size.x, (int)size.y);
}

void gl::ViewportEditorWindow::onDraw(Editor* editor)
{
	mOldImGui3DContext = ImGui3D::GImGui3D;
	ImGui3D::SetContext(mImGui3DContext);
	ImGui3D::NewFrame(editor->viewportCamera->viewMatrix, editor->viewportCamera->GetProjectionMatrix(), ImGui::GetCurrentWindow()->ID);
	glViewport(0, 0, (int)size.x, (int)size.y);

	mGeometryFrameBuffer->bind();
	mGeometryFrameBuffer->clearColorAttachment(0, editor->clearColor);
	mGeometryFrameBuffer->clearDepthBuffer();

	// Update viewport camera
	if (ImGui::IsWindowHovered()) {
		editor->viewportControl->update(editor->viewportCamera, true);
	}

	glEnable(GL_DEPTH_TEST);
	for (auto mesh : editor->getObjects()) {
		if (mesh->visible) {
			mesh->render(editor->viewportCamera);
			mesh->handleIO(editor->viewportCamera, ImGui::GetIO());
		}
	}
	mFrameBuffer->bind();
	if (mLastTonemapping != editor->toneMapping) {
		mTonemappingShader->setDefine("HDR_MAPPING_TYPE", static_cast<int>(editor->toneMapping));
		mTonemappingShader->update();
	}
	if (editor->toneMapping != ToneMapping::Linear || editor->gammaCorrection) {
		fullscreenTriangle(0, 0, editor->viewportCamera->ScreenWidth, editor->viewportCamera->ScreenHeight,
			*mTonemappingShader,
			"gamma", editor->gammaCorrection ? editor->gamma : 1.0f,
			"hdr", editor->hdr,
			"renderTexture", mGeometryFrameBuffer->getRenderTexture(0));
	}
	else {
		displayTexture(0, 0, editor->viewportCamera->ScreenWidth, editor->viewportCamera->ScreenHeight, mGeometryFrameBuffer->getRenderTexture(0));
	}

	// Clear id map and render ImGui3D stuff
	mFrameBuffer->clearColorAttachment(1, glm::vec4(0, 0, 0, 1));
	for (auto mesh : editor->getObjects()) {
		if (mesh->visible) {
			mesh->drawViewportUI(editor->viewportCamera);
			if (std::dynamic_pointer_cast<gl::TriangleMesh>(mesh) != nullptr) {
				ImGui3D::TransformGizmo(mesh->ModelMatrix);
			}
		}
	}
	mFrameBuffer->clearDepthBuffer();
	ImGui3D::Render();
	mFrameBuffer->unbind();

	ImGui::GetWindowDrawList()->AddImage(
		*mFrameBuffer->getRenderTexture(0),
		ImGui::GetWindowPos(),
		ImGui::GetWindowPos() + ImGui::GetWindowSize(),
		ImVec2(0, 1), ImVec2(1, 0));


	ImGui3D::SetContext(mOldImGui3DContext);
}

gl::LoggingEditorWindow::LoggingEditorWindow(const std::string& title, EditorWindowRegion defaultRegion) :
	gl::EditorWindow(title, defaultRegion),
	maxLogSize(100),
	mMessageJustLogged(false)
{
	Logging::RegisterEndpoint(this);
	icons[0] = std::string(ICON_FA_INFO_CIRCLE);
	icons[1] = std::string(ICON_FA_CHECK_CIRCLE);
	icons[2] = std::string(ICON_FA_EXCLAMATION_TRIANGLE);
	icons[3] = std::string(ICON_FA_EXCLAMATION_CIRCLE);

	mEnabled[0] = mEnabled[1] = mEnabled[2] = mEnabled[3] = true;
}

gl::LoggingEditorWindow::~LoggingEditorWindow()
{
	Logging::RemoveEndpoint(this);
}

void gl::LoggingEditorWindow::onMessage(const LogMessage& msg)
{
	if (mLogs.size() >= maxLogSize) {
		mLogs.pop_front();
	}
	mLogs.push_back({ msg.level, msg.msg });
	mMessageJustLogged = true;
}

void gl::LoggingEditorWindow::onDraw(Editor* editor)
{
	const char* types[] = {
		"Info",
		"Success",
		"Warning",
		"Error"
	};

	const ImVec4 colors[4] = {
		ImVec4(0, 1, 1, 1),
		ImVec4(0, 1, 0, 1),
		ImVec4(1, 1, 0, 1),
		ImVec4(1, 0, 0, 1)
	};

	for (int i = 0; i < 4; ++i) {
		bool wasDisabled = !mEnabled[i];
		if (wasDisabled) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		std::string text = icons[i] + " " + types[i];
		if (ImGui::Button(text.c_str())) {
			mEnabled[i] = !mEnabled[i];
		}
		if (wasDisabled) {
			ImGui::PopStyleVar();
		}
		if (i != 3) {
			ImGui::SameLine();
		}
	}

	if (ImGui::BeginChild("##Messages")) {
		for (size_t i = 0; i < mLogs.size(); ++i) {
			int t = mLogs[i].first;
			if (!mEnabled[t]) { continue; }
			ImGui::Separator();
			ImGui::TextColored(colors[t], icons[t].c_str());
			ImGui::SameLine();
			ImGui::TextWrapped(mLogs[i].second.c_str());
		}
		if (mMessageJustLogged) {
			ImGui::SetScrollHere(1.0f);
			mMessageJustLogged = false;
		}
	}
	ImGui::EndChild();

}
