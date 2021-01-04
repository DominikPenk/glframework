#pragma once

#include "glpp/imgui.hpp"

namespace ImGui {

	struct ImVerticalTabBar;

	struct ImVerticalTabBar {
		ImVerticalTabBar() :
			activeTabID(0),
			currentTabID(0),
			tabChildID(0) {}

		static ImVerticalTabBar*        CurrentVerticalTabBar;
		static ImPool<ImVerticalTabBar> VerticalTabBars;
		
		ImGuiID activeTabID;
		ImGuiID currentTabID;
		ImGuiID tabChildID;
	};

	bool BeginVerticalTabBar(const char* id);
	void EndVerticalTabBar();

	bool BeginVerticalTabItem(const char* label);
	void EndVerticalTabItem();
}