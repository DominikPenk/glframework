#pragma once

#include <string>

struct ImGuiStyle;

namespace ImGui {
	void DarkStyle(ImGuiStyle* dst = NULL);
	void VisualStudioStyle(ImGuiStyle* dst = NULL);
	void PhotoshopStyle(ImGuiStyle* dst = NULL);
}