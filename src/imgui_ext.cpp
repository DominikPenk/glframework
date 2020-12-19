#include "glpp/imgui_ext.hpp"
#include "glpp/imgui.hpp"
#include "glpp/texture.hpp"

#include <glm/gtx/vector_angle.hpp>

#include <cmath>
#include <iostream>

void ImGui::Image(std::shared_ptr<gl::Texture> tex, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGui::Image(*tex, size, uv0, uv1, tint_col, border_col);
}

bool ImGui::ImageButton(std::shared_ptr<gl::Texture> tex, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	return ImGui::ImageButton(*tex, size, uv0, uv1, frame_padding, bg_col, tint_col);
}

bool ImGui::InputUInt(const char* label, uint32_t* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%u";
	return InputScalar(label, ImGuiDataType_U32, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);

}

bool ImGui::InputInt64(const char* label, int64_t* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%lld";
	return InputScalar(label, ImGuiDataType_S64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

bool ImGui::InputUInt64(const char* label, uint64_t* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%llu";
	return InputScalar(label, ImGuiDataType_U64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
}

ImU32 ImGui::ApplyAlpha(ImU32 col, float alpha)
{
	if (alpha >= 1.0f)
		return col;
	ImU32 a = (col & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT;
	a = (ImU32)(a * alpha);
	return (col & ~IM_COL32_A_MASK) | (a << IM_COL32_A_SHIFT);

}

bool ImGui::SquareHandle(const char* id, float pos[2], ImVec2 size, ImU32 color)
{
	const ImVec2 offset = ImGui::GetWindowPos();
	const ImVec2 oldCursorPos = ImGui::GetCursorPos();
	const ImVec2 localPos(pos[0], pos[1]);

	ImGui::SetCursorPos(localPos - size * 0.5f);
	ImGui::InvisibleButton(id, size);
	ImGui::GetWindowDrawList()->AddRectFilled(offset + localPos - size * 0.5f, offset + localPos + size * 0.5f, color);
	bool change = false;
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		pos[0] += delta.x;
		pos[1] += delta.y;
		change = true;
	}

	// Reset cursor
	ImGui::SetCursorPos(oldCursorPos);
	return change;

}

bool ImGui::DotHandle(const char* id, float pos[2], float radius, ImU32 color)
{
	const ImVec2 offset = ImGui::GetWindowPos();
	const ImVec2 oldCursorPos = ImGui::GetCursorPos();
	const ImVec2 localPos(pos[0], pos[1]);

	ImGui::SetCursorPos(localPos - ImVec2(radius, radius));
	ImGui::InvisibleButton(id, ImVec2(radius, radius) * 2.0f);
	ImGui::GetWindowDrawList()->AddCircleFilled(localPos + offset, radius, color);
	bool change = false;
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		pos[0] += delta.x;
		pos[1] += delta.y;
		change = true;
	}

	// Reset cursor
	ImGui::SetCursorPos(oldCursorPos);
	return change;
}

bool ImGui::AxisAlignedBox(const char* id, float center[2], float size[2], ImU32 color, float alpha, float handleSize)
{
	ImGui::PushID(id);

	ImVec2 hSize(handleSize, handleSize);
	ImVec2 c(center[0], center[1]);

	ImVec2 p00(center[0] - size[0] * 0.5f, center[1] - size[1] * 0.5f);
	ImVec2 p01(center[0] + size[0] * 0.5f, center[1] - size[1] * 0.5f);
	ImVec2 p10(center[0] - size[0] * 0.5f, center[1] + size[1] * 0.5f);
	ImVec2 p11(center[0] + size[0] * 0.5f, center[1] + size[1] * 0.5f);

	ImVec2 windowPos = ImGui::GetWindowPos();

	auto DL = ImGui::GetWindowDrawList();
	if (alpha > 0.f) {
		DL->AddRectFilled(windowPos + p00, windowPos + p11, ApplyAlpha(color, 0.25));
	}
	DL->AddRect(windowPos + p00, windowPos + p11, color, 0.f, 15, 3.f);

	ImVec2 min = p00;
	ImVec2 max = p11;

	bool deactivated = false;

	bool centerChanged = ImGui::DotHandle("##Center", center, handleSize, color);
	deactivated = deactivated || ImGui::IsItemDeactivated();

	bool tlChanged = ImGui::SquareHandle("##TL", &min.x, hSize, color);
	deactivated = deactivated || ImGui::IsItemDeactivated();
	bool trChanged = ImGui::SquareHandle("##TR", &p01.x, hSize, color);
	deactivated = deactivated || ImGui::IsItemDeactivated();
	bool blChanged = ImGui::SquareHandle("##BL", &p10.x, hSize, color);
	deactivated = deactivated || ImGui::IsItemDeactivated();
	bool brChanged = ImGui::SquareHandle("##BR", &max.x, hSize, color);
	deactivated = deactivated || ImGui::IsItemDeactivated();

	if (trChanged) {
		min.y = p01.y;
		max.x = p01.x;
	}
	if (blChanged) {
		min.x = p10.x;
		max.y = p10.y;
	}

	if (tlChanged || trChanged || blChanged || brChanged) {
		center[0] = 0.5f * (min.x + max.x);
		center[1] = 0.5f * (min.y + max.y);

		size[0] = max.x - min.x;
		size[1] = max.y - min.y;
	}

	if (deactivated) {
		size[0] = std::abs(size[0]);
		size[1] = std::abs(size[1]);
	}

	ImGui::PopID();
	return centerChanged || tlChanged || trChanged || blChanged || brChanged;
}

bool ImGui::Rotation(const char* id, float* angle, float pivot[2], float radius, ImU32 col)
{
	const float c = std::cos(glm::radians(*angle));
	const float s = std::sin(glm::radians(*angle));
	const ImVec2 dir(s, -c);

	const ImVec2 offset = ImGui::GetWindowPos();
	const ImVec2 oldCursorPos = ImGui::GetCursorPos();
	const ImVec2 localPos(pivot[0], pivot[1]);

	const ImVec2 globalPivot = localPos + offset;
	const ImVec2 globalHandle = globalPivot + dir * radius;

	ImGui::GetWindowDrawList()->AddLine(globalPivot, globalHandle, col, 2.f);
	ImGui::GetWindowDrawList()->AddCircleFilled(globalPivot, 5.f, col);

	ImGui::SetCursorPos(globalHandle - ImVec2(5, 5));
	ImGui::InvisibleButton(id, ImVec2(10, 10));
	ImGui::GetWindowDrawList()->AddRectFilled(globalHandle - ImVec2(5, 5), globalHandle + ImVec2(5, 5), col);

	bool change = false;
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		glm::vec2 dir = imGui2Glm(ImGui::GetIO().MousePos - globalPivot);
		const float l = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (l > 1e-6f) {
			dir /= l;
			*angle = glm::degrees(glm::orientedAngle(glm::vec2(0, -1), dir));
			if (*angle < 0) {
				*angle += 360.f;
			}
			change = true;
		}
	}

	ImGui::SetCursorPos(oldCursorPos);
	return change;
}

bool ImGui::BeginCanvas(const char* name, ImVec2 position, ImVec2 size)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos() + position);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking;
	bool success = ImGui::Begin(name, NULL, windowFlags);
	ImGui::PopStyleVar(2);
	return success;
}

bool ImGui::BeginCanvasFullscreen(const char* name)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDocking;
	bool success = ImGui::Begin(name, NULL, windowFlags);
	ImGui::PopStyleVar(2);
	return success;
}
