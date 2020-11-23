#include "glpp/imgui_ext.hpp"
#include "glpp/imgui.hpp"
#include "glpp/texture.hpp"

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
