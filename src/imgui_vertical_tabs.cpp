#include "glpp/imgui_vertical_tabs.hpp"

#include <iostream>

ImPool<ImGui::ImVerticalTabBar> ImGui::ImVerticalTabBar::VerticalTabBars;
ImGui::ImVerticalTabBar* ImGui::ImVerticalTabBar::CurrentVerticalTabBar = NULL;

bool ImGui::BeginVerticalTabBar(const char* str_id)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

	// Currently we cannot nest vertical tabs
    if (ImVerticalTabBar::CurrentVerticalTabBar != NULL)
    {
        IM_ASSERT_USER_ERROR(ImVerticalTabBar::CurrentVerticalTabBar == NULL, "Nested vertical tabs are not supported (did you miss to call EndVerticalTabBar()?)");
        return false;
    }

    ImGuiID id = window->GetID(str_id);
    ImVerticalTabBar* tabBar = ImVerticalTabBar::VerticalTabBars.GetOrAddByKey(id);
    ImVerticalTabBar::CurrentVerticalTabBar = tabBar;

    ImGui::PushID(id);

    // Create the child for rendering later on
    tabBar->tabChildID = window->GetID("TabBar");

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_TitleBg));
    ImGui::BeginChild(tabBar->tabChildID, ImVec2(60, 0));       // FIXME: make width dynamic (or set it as parameter)
    ImGui::PopStyleColor(1);

    // Draw Seperator
    ImVec2 tl = ImGui::GetWindowPos() + ImVec2(59, 0);
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator));
    ImGui::GetWindowDrawList()->AddLine(tl, tl + ImVec2(0, ImGui::GetWindowHeight()), col);
    
    ImGui::EndChild();
}

void ImGui::EndVerticalTabBar()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return;

    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar;
    if (tabBar == NULL) {
        IM_ASSERT_USER_ERROR(tabBar != NULL, "Mismatched BeginVerticalTabBar()/EndVerticalTabBar()!");
        return;
    }

    if (tabBar->currentTabID != 0) {
        IM_ASSERT_USER_ERROR(tabBar->currentTabID != NULL, "Missing EndVerticalTabBarItem()!");
        return;
    }

    // Pop the tab id from the stack
    ImGui::PopID();
    ImVerticalTabBar::CurrentVerticalTabBar = NULL;
}

bool ImGui::BeginVerticalTabItem(const char* label)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar;
    if (tabBar == NULL)
    {
        IM_ASSERT_USER_ERROR(tabBar, "Needs to be called between BeginVerticalTabBar() and EndVerticalTabBar()!");
        return false;
    }

    if (tabBar->currentTabID != 0) {
        IM_ASSERT_USER_ERROR(tabBar->currentTabID == 0, "Mismatched BeginVerticalTabItem()/EndVerticalTabItem()!");
        return false;
    }

    ImGuiID thisTabID = window->GetID(label);
    if (tabBar->activeTabID == 0) {
        tabBar->activeTabID = thisTabID;
    }

    // Render the button in the button child
    ImGui::BeginChild(tabBar->tabChildID);
    // Style like tabs 
    // FIXME: Make active tab more visible
    bool active = thisTabID == tabBar->activeTabID;
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(active ? ImGuiCol_TabUnfocusedActive : ImGuiCol_TabUnfocused));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_TabHovered));
    if (ImGui::Button(label)) {
        tabBar->activeTabID = thisTabID;
    }
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    if (thisTabID == tabBar->activeTabID) {
        ImGui::SameLine();
        ImGui::BeginChild(thisTabID);
        tabBar->currentTabID = thisTabID;
        return true;
    }
    return false;
}

void ImGui::EndVerticalTabItem()
{
    ImGuiContext& g = *GImGui;
    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar;
    if (tabBar == NULL)
    {
        IM_ASSERT_USER_ERROR(tabBar, "Needs to be called between BeginVerticalTabBar() and EndVerticalTabBar()!");
        return;
    }

    if (tabBar->currentTabID == 0) {
        IM_ASSERT_USER_ERROR(tabBar->currentTabID != 0, "Mismatched BeginVerticalTabItem()/EndVerticalTabItem()!");
        return;
    }

    if (tabBar->activeTabID != tabBar->currentTabID) {
        IM_ASSERT_USER_ERROR(tabBar->activeTabID == tabBar->currentTabID, "You should not call EndVerticalTabItem() if the tab is not the active one");
    }

    ImGui::EndChild();
    tabBar->currentTabID = 0;
}
