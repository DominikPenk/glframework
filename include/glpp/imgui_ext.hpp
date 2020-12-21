#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "imgui.h"

namespace gl {
	class Texture;
}

namespace ImGui {

	void Image(std::shared_ptr<gl::Texture> tex, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	bool ImageButton(std::shared_ptr<gl::Texture> tex, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool InputUInt(const char* label, uint32_t* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	bool InputInt64(const char* label, int64_t* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	bool InputUInt64(const char* label, uint64_t* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);

	template<typename T>
	bool DropdownSelect(const char* label, T* current, const std::map<T, std::string>& choices);
	bool DropdownSelect(const char* label, int* current, const std::vector<std::string>& choices);


	ImU32 ApplyAlpha(ImU32 col, float alpha);

	// A canvas is just a wrapper around ImGui::Begin setting appropriate flags
	bool BeginCanvas(const char* name, ImVec2 position, ImVec2 size);
	bool BeginCanvasFullscreen(const char* name);
	const auto EndCanvas = End;

	// The functions below expect positions relative to the top left corner of the current window
	// They also do not advance the cursor position
	// TODO: At some point I want to use the internal functions (e.g. ButtonBehavior) instead of highlevel ImGui functions
	// (See: https://github.com/ocornut/imgui/issues/2992#issuecomment-576160676)

	// A square handle to adust positions
	bool SquareHandle(const char* id, float pos[2], ImVec2 size, ImU32 color);
	bool DotHandle(const char* id, float pos[2], float radius, ImU32 color);
	bool AxisAlignedBox(const char* id, float center[2], float size[2], ImU32 color, float alpha = 0.25f, float handleSize = 10.f);

	// A Rotation gizmo. The angle is given in degrees
	bool Rotation(const char* id, float* angle, float pivot[2], float radius, ImU32 color);

}

#include "imgui_ext.inl.hpp"