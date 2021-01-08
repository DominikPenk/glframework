#include "glpp/imgui_vertical_tabs.hpp"

#include <iostream>

ImPool<ImGui::ImVerticalTabBar> ImGui::ImVerticalTabBar::VerticalTabBars;
ImVector<ImGui::ImVerticalTabBar*> ImGui::ImVerticalTabBar::VerticalTabBarStack;

bool ImGui::BeginVerticalTabBar(ImGuiID id, ImGuiVerticalTabFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImVerticalTabBar* tabBar = ImVerticalTabBar::VerticalTabBars.GetOrAddByKey(id);
    tabBar->tabColumn = flags & ImGuiVerticalTabFlags_Right ? 1 : 0;
    ImVerticalTabBar::VerticalTabBarStack.push_back(tabBar);

    ImGui::PushID(id);

    const ImGuiTableFlags tableFlags = 0;
    const bool tableVisible = BeginTable("##VerticalTabLayout", 2, tableFlags, GetContentRegionAvail());
    if (!tableVisible) { PopID(); }
    else { 
        TableSetupColumn("Col0", tabBar->tabColumn == 0 ? ImGuiTableColumnFlags_WidthAuto : ImGuiTableColumnFlags_WidthStretch);
        TableSetupColumn("Col1", tabBar->tabColumn == 1 ? ImGuiTableColumnFlags_WidthAuto : ImGuiTableColumnFlags_WidthStretch);
        TableNextRow(); 
        TableSetColumnIndex(tabBar->tabColumn);
        tabBar->cursorPos = GetCursorPos();
    }
    return tableVisible;
}

bool ImGui::BeginVerticalTabBar(const char* str_id, ImGuiVerticalTabFlags flags) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    if (window->SkipItems)
        return false;

    ImGuiID id = window->GetID(str_id);

    return BeginVerticalTabBar(id, flags);
}

void ImGui::EndVerticalTabBar()
{
    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar();
    if (tabBar == NULL) {
        IM_ASSERT_USER_ERROR(tabBar != NULL, "Mismatched BeginVerticalTabBar()/EndVerticalTabBar()!");
        return;
    }

    // Pop the tab id from the stack
    ImGui::EndTable();
    ImGui::PopID();
    ImVerticalTabBar::VerticalTabBarStack.pop_back();
}

bool ImGui::BeginVerticalTabItem(const char* label)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;

    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar();
    if (tabBar == NULL)
    {
        IM_ASSERT_USER_ERROR(tabBar, "Needs to be called between BeginVerticalTabBar() and EndVerticalTabBar()!");
        return false;
    }

    ImGuiID thisTabID = window->GetID(label);
    if (tabBar->activeTabID == 0) {
        tabBar->activeTabID = thisTabID;
    }

    // Render the Button to the left most column
    const bool active = thisTabID == tabBar->activeTabID;
    if (TableSetColumnIndex(tabBar->tabColumn)) {
        // Style like tabs 
        // FIXME: Make active tab more visible
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(active ? ImGuiCol_TabUnfocusedActive : ImGuiCol_TabUnfocused));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_TabActive));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_TabHovered));
        SetCursorPos(tabBar->cursorPos);
        if (ImGui::Button(label)) {
            tabBar->activeTabID = thisTabID;
        }
        tabBar->cursorPos = GetCursorPos();
        ImGui::PopStyleColor(3);
    }
    return active && TableSetColumnIndex((tabBar->tabColumn + 1) % 2);
}

void ImGui::EndVerticalTabItem()
{
    ImGuiContext& g = *GImGui;
    ImVerticalTabBar* tabBar = ImVerticalTabBar::CurrentVerticalTabBar();
    if (tabBar == NULL)
    {
        IM_ASSERT_USER_ERROR(tabBar, "Needs to be called between BeginVerticalTabBar() and EndVerticalTabBar()!");
        return;
    }
}
