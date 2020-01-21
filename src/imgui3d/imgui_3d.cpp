#include "imgui3d/imgui_3d.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_internal.h"


#define GLMCOLCHAR2FLOAT(r, g, b, a) glm::vec4(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b)/255, static_cast<float>(a) / 255);

namespace ImGui3D {
	std::shared_ptr<ImGui3DContext> GImGui3D = nullptr;

	std::shared_ptr<ImGui3DContext> CreateContext()
	{
		std::shared_ptr<ImGui3DContext> ctx = std::make_shared<ImGui3DContext>();
		ctx->drawCommands.push_back(std::make_shared<DrawCommand>());
		if (GImGui3D == nullptr) {
			SetContext(ctx);
		}
		return ctx;
	}

	void SetContext(std::shared_ptr<ImGui3DContext> ctx)
	{
		GImGui3D = ctx;
	}

	void NewFrame(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix)
	{
		IM_ASSERT(GImGui3D != nullptr && "No current context. Did you call ImGui3D::CreateContext() or ImGui3D::SetCurrentContext()?");
		ImGui3DContext& g = *GImGui3D;
		ImGuiIO& io = ImGui::GetIO();

		g.ModelMatrix = glm::mat4(1);
		g.ViewMatrix = ViewMatrix;
		g.ProjectionMatrix = ProjectionMatrix;
		g.ViewProjectionMatrix = g.ProjectionMatrix * g.ViewMatrix;
		g.ViewPerspectiveMatrixInverse = glm::inverse(g.ViewProjectionMatrix);
		g.CameraPosition = glm::inverse(g.ViewMatrix) * glm::vec4(0, 0, 0, 1);

		g.drawCommands.resize(1);
		g.drawCommands.back()->Clear();

		g.SeedStack.push_back(0);

		// Check if we should signal mouse and keyboard captureing
		if (g.KeepCaptureFocus) {
			io.WantCaptureKeyboard = true;
			io.WantCaptureMouse = true;
			// Make sure the AcitveId in last frame stays active
			//g.ActiveId = g.ActiveIdPreviousFrame;
		}
		else {
			// Clear current activeId
			g.ActiveIdPreviousFrame = g.ActiveId;
			g.ActiveId = 0;
		}

		// Look for ids under the mouse cursor if neccessary
		ImRect windowRect(ImVec2(0, 0), g.ScreenSize);
		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse && windowRect.Contains(io.MousePos)) {
			g.ActiveIdColor = g.GetHoveredIdImpl(io.MousePos);
			g.ActiveIdPreviousFrame = g.ActiveId;
			g.ActiveId = ColorToID(g.ActiveIdColor.r, g.ActiveIdColor.g, g.ActiveIdColor.b);
		}


	}

	void Render()
	{
		IM_ASSERT(GImGui3D != nullptr && "No current context. Did you call ImGui3D::CreateContext() or ImGui3D::SetCurrentContext()?");
		ImGui3DContext& g = *GImGui3D;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (auto cmd : g.drawCommands) {
			cmd->execute();
		}
		glDisable(GL_BLEND);
	}

	bool IsItemActive()
	{
		return GImGui3D->ActiveId == GImGui3D->CurrentId && GImGui3D->KeepCaptureFocus;
	}

	bool IsItemClicked()
	{
		const ImGuiIO& io = ImGui::GetIO();
		const ImGui3DContext& g = *ImGui3D::GImGui3D;
		return g.ActiveId == g.CurrentId && io.MouseClicked[0];
	}

	ImGuiID GetID(const std::string& str, bool keepFocus)
	{
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		const ImGuiID seed = g.SeedStack.back();
		const ImGuiID id = (ImHashStr(str.c_str(), 0, seed) % (256 * 256 * 256)) + 1;
		g.CurrentId = id;
		if (keepFocus) {
			ImGuiIO& io = ImGui::GetIO();
			if (io.MouseClicked[0] && g.ActiveId == id) {
				TakeIOFocus(id);
			}
			else if (io.MouseReleased[0] && g.ActiveId == id) {
				ReleaseIOFocus(id);
			}
		}
		return id;
	}

	ImGuiID GetID(const void* data, bool keepFocus) {
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		const ImGuiID seed = g.SeedStack.back();
		const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(data);
		const ImGuiID id = (ImHashData(&address, sizeof(address), seed) % (256 * 256 * 256)) + 1;
		g.CurrentId = id;
		if (keepFocus) {
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::IsMouseClicked(0) && g.ActiveId == id) {
				TakeIOFocus(id);
			}
			else if (io.MouseReleased[0] && g.ActiveId == id) {
				ReleaseIOFocus(id);
			}
		}
		return id;
	}

	ImGuiID ColorToID(unsigned char r, unsigned char g, unsigned char b)
	{
		return (ImGuiID)r + 256 * ((ImGuiID)g + 256 * (ImGuiID)b);
	}

	glm::vec4 IDToColor(ImGuiID id) {
		ImGuiID b = id / (256 * 256);
		ImGuiID g = (id / 256) % 256;
		ImGuiID r = id % 256;
		return glm::vec4((float)r / 255, (float)g / 255, (float)b / 255, 1);
	}

	void PushID(ImGuiID id)
	{
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		const void* id_ptr = (void*)(intptr_t)id;
		const ImGuiID seed = GetID(&id_ptr, false);
		g.SeedStack.push_back(seed);
	}

	void PopID()
	{
		ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
		g.SeedStack.pop_back();
		assert(g.SeedStack.size() >= 1);
	}

	void TakeIOFocus(ImGuiID id)
	{
		GImGui3D->KeepCaptureFocus = true;
		GImGui3D->ActiveId = id;
	}

	void ReleaseIOFocus(ImGuiID id)
	{
		//assert(GImGui3D->ActiveId == id && GImGui3D->KeepCaptureFocus);
		GImGui3D->KeepCaptureFocus = false;
	}

	ImGui3DContext::ImGui3DContext() :
		KeepCaptureFocus(false),
		ActiveId(0),
		ActiveIdPreviousFrame(0),
		Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "imgui3d.glsl"),
		ModelMatrix(1),
		ViewMatrix(0),
		ProjectionMatrix(0),
		ViewProjectionMatrix(0),
		Style()
	{
	}

	glm::vec4 ImGui3DStyle::getColorForID(ImGuiID id, ImGui3DColors element)
	{
		const ImGui3DContext& g = *GImGui3D;
		return g.ActiveId == id ? Colors[static_cast<int>(element) + 1] : Colors[static_cast<int>(element)];
	}

	glm::vec4 ImGui3DStyle::getColor(ImGui3DColors element)
	{
		const ImGui3DContext& g = *GImGui3D;
		return g.ActiveId == g.CurrentId ? Colors[static_cast<int>(element) + 1] : Colors[static_cast<int>(element)];
	}

	ImGui3DStyle::ImGui3DStyle()
	{
		GizmoSize                    = 120;
		VertexSize                   = 20;
		RotationGizmoSegments        = 48;
		TranslationGizmoInnerPadding = .1f;
		TranslationGizmoPlaneSize    = .4f;

		LightRadius                  = 25;
		SpotLightConeHeight          = 250;
		LightDirectionLength         = 375;
		LightHandleSize              =  12;

		CubeMapRadius                =  45;
		CubeMapSize                  =  25;


		// Set up colors
		Colors[ImGui3DCol_xAxis]          = GLMCOLCHAR2FLOAT(210,  50, 110, 255);
		Colors[ImGui3DCol_yAxis]          = GLMCOLCHAR2FLOAT(140, 210,  50, 255);
		Colors[ImGui3DCol_zAxis]          = GLMCOLCHAR2FLOAT( 80, 100, 210, 255);
		Colors[ImGui3DCol_xAxis_selected] = GLMCOLCHAR2FLOAT(255,  51,  82, 255);
		Colors[ImGui3DCol_yAxis_selected] = GLMCOLCHAR2FLOAT(140, 255,   0, 255);
		Colors[ImGui3DCol_zAxis_selected] = GLMCOLCHAR2FLOAT( 40, 144, 255, 255);

		Colors[ImGui3DCol_xyPlane]          = Colors[ImGui3DCol_zAxis];
		Colors[ImGui3DCol_xyPlane_selected] = Colors[ImGui3DCol_zAxis_selected];
		Colors[ImGui3DCol_xzPlane]          = Colors[ImGui3DCol_yAxis];
		Colors[ImGui3DCol_xzPlane_selected] = Colors[ImGui3DCol_yAxis_selected];
		Colors[ImGui3DCol_yzPlane]          = Colors[ImGui3DCol_xAxis];
		Colors[ImGui3DCol_yzPlane_selected] = Colors[ImGui3DCol_xAxis_selected];

		Colors[ImGui3DCol_xRotation_circle] = Colors[ImGui3DCol_xAxis];
		Colors[ImGui3DCol_xRotation_circle].a = 0.5f;
		Colors[ImGui3DCol_yRotation_circle] = Colors[ImGui3DCol_yAxis];
		Colors[ImGui3DCol_yRotation_circle].a = 0.5f;
		Colors[ImGui3DCol_zRotation_circle] = Colors[ImGui3DCol_zAxis];
		Colors[ImGui3DCol_zRotation_circle].a = 0.5f;

		Colors[ImGui3DCol_Vertex]          = GLMCOLCHAR2FLOAT(227, 126,  15, 255);
		Colors[ImGui3DCol_Vertex_selected] = GLMCOLCHAR2FLOAT(227, 227, 227, 255);

		Colors[ImGui3DCol_Editor_Widget]          = GLMCOLCHAR2FLOAT(  0,   0,  0, 255);
		Colors[ImGui3DCol_Editor_Widget_selected] = GLMCOLCHAR2FLOAT(255, 160, 40, 255);
	}
}

