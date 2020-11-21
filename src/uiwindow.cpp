#include "glpp/uiwindow.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "glpp/renderer.hpp"
#include "glpp/IconsFontAwesome5.h"
#include "glpp/texture.hpp"
#include "glpp/imgui3d/imgui_3d.h"


namespace gl {
namespace impl {

	static inline void AspectImage(GLuint tid, const int width, const int height, const ImVec2 maxSize, const ImVec2 uv0 = ImVec2(0, 0), const ImVec2 uv1 = ImVec2(1, 1), const ImVec4 tint_color = ImVec4(1, 1, 1, 1)) {
		float sx = (float)maxSize.x / (float)width;
		float sy = (float)maxSize.y / (float)height;
		float s = std::min(sx, std::min(sy, 1.0f));
		ImGui::Image((GLuint*)tid, ImVec2((float)width, (float)height) * s, uv0, uv1, tint_color);
	}

}
}

gl::UIWindow::UIWindow(std::string title) :
	title(title),
	open(false)
{
}

void gl::UIWindow::draw(Renderer* env)
{
	ImGui::Begin(title.c_str(), &open);
	onDraw(env);
	ImGui::End();
}

void gl::UIWindow::preDraw(Renderer* env)
{
}

void gl::UIWindow::viewportDraw(Renderer* env)
{
}

gl::GenericUIWindow::GenericUIWindow(std::string title, std::function<void(Renderer*)> drawFn) :
	UIWindow(title),
	mDrawFn(drawFn)
{
}

void gl::GenericUIWindow::onDraw(Renderer* env)
{
	mDrawFn(env);
}

gl::OutlinerWindow::OutlinerWindow() :
	UIWindow("Outliner")
{
}

void gl::OutlinerWindow::onDraw(Renderer* env)
{
	int idx = 0;
	for (auto obj : env->objects()) {
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

gl::RendererDebugWindow::RendererDebugWindow() :
	gl::UIWindow("Debug Window")
{
}

void gl::RendererDebugWindow::onDraw(Renderer* env)
{
	const char* hdrmappings[] = {
		"Linear",
		"Reinhard",
		"Haarm-Peter Duiker",
		"Jim Hejl, Richard Burgess-Dawson",
		"Uncharted 2" 
	};

	std::shared_ptr<gl::Texture> colorTexture = env->framebuffer()->getRenderTexture(0);
	std::shared_ptr<gl::Texture> idTexture = env->framebuffer()->getRenderTexture(1);

	if (ImGui::BeginCombo("HDR Mapping", hdrmappings[(int)env->toneMapping()])) {
		for (int i = 0; i < IM_ARRAYSIZE(hdrmappings); ++i) {
			bool isSelected = i == (int)env->toneMapping();
			if (ImGui::Selectable(hdrmappings[i], isSelected)) {
				env->setToneMapping(static_cast<Renderer::ToneMapping>(i));
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Checkbox("Gamma Correction", &env->gammaCorrection);
	if (env->gammaCorrection) {
		ImGui::DragFloat("Gamma", &env->gamma, .1f, 0.1f, 4.0f);
	}
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	impl::AspectImage(colorTexture->id, colorTexture->cols, colorTexture->rows, ImVec2(500, 500), ImVec2(0, 1), ImVec2(1, 0));
	impl::AspectImage(idTexture->id, colorTexture->cols, colorTexture->rows, ImVec2(500, 500), ImVec2(0, 1), ImVec2(1, 0));

	// Get color under the cursor
	ImVec2 mouse = ImGui::GetIO().MousePos;
	glm::uvec4 color = env->framebuffer()->readColorPixel((int)mouse.x, (int)mouse.y, 1);
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
	ImGui::Text("Active id: %d (color: %d, %d, %d)", g.ActiveId, g.ActiveIdColor.r, g.ActiveIdColor.g, g.ActiveIdColor.b);
}
