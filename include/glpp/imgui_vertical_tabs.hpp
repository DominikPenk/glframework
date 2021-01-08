#pragma once

#include "glpp/imgui.hpp"

typedef int ImGuiVerticalTabFlags;

enum ImGuiVerticalTabFlags_ {
	ImGuiVerticalTabFlags_None  = 0,
	ImGuiVerticalTabFlags_Right = 1 << 1,	// Tabs on the right side
};

namespace ImGui {


	struct ImVerticalTabBar;

	struct ImVerticalTabBar {
		ImVerticalTabBar() :
			activeTabID(0),
			cursorPos(0, 0),
			tabColumn(0) {}

		ImVector<const char*> labels;

		static ImPool<ImVerticalTabBar>    VerticalTabBars;
		static ImVector<ImVerticalTabBar*> VerticalTabBarStack;

		static ImVerticalTabBar* CurrentVerticalTabBar() { return VerticalTabBarStack.empty() ? NULL : VerticalTabBarStack[VerticalTabBarStack.Size - 1]; }
		
		ImGuiID activeTabID;
		ImVec2  cursorPos;
		int     tabColumn;
	};

	bool BeginVerticalTabBar(ImGuiID id, ImGuiVerticalTabFlags flags = 0);
	bool BeginVerticalTabBar(const char* str_id, ImGuiVerticalTabFlags flags = 0);
	void EndVerticalTabBar();

	bool BeginVerticalTabItem(const char* label);
	void EndVerticalTabItem();
}