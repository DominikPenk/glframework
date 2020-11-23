#pragma once
#include <memory>
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
}