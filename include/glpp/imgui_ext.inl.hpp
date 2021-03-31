#include "imgui_ext.hpp"

template<typename T>
inline bool ImGui::SelectButtonGroupH(const char* label, T* current, const std::map<T, std::string>& choices)
{
	bool changed = false;
	ImGui::PushID(label);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	int id = 0;
	for (const auto& [ key, label ]: choices) {
		ImGui::PushID(id++);
		bool isSelected = *current == key;
		if (isSelected) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		}
		if (ImGui::Button(label.c_str())) {
			*current = key;
			changed = true;
		}
		if (isSelected) {
			ImGui::PopStyleColor();
		}
		ImGui::PopID();
		ImGui::SameLine();
	}
	ImGui::NewLine();
	ImGui::PopStyleVar();
	if (std::strlen(label) != 0) {
		ImGui::SameLine();
		ImGui::TextUnformatted(label);
	}
	ImGui::PopID();
	return changed;
}

template<typename T>
inline bool ImGui::DropdownSelect(const char* label, T* current, const std::map<T, std::string>& choices)
{
	bool changed = false;
	if (ImGui::BeginCombo(label, choices.at(*current).c_str())) {
		int id = 0;
		for (auto& [key, value] : choices) {
			ImGui::PushID(id++);
			bool isSelected = *current == key;
			if (ImGui::Selectable(value.c_str(), isSelected)) {
				*current = key;
				changed = true;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return changed;
}

template<typename T, typename LabelGenerator>
inline bool ImGui::DropdownSelect(const char* label, T* current, const std::vector<T>& choices, LabelGenerator labelGenerator)
{
	bool changed = false;
	std::string currentLabel = labelGenerator(*current);
	if (ImGui::BeginCombo(label, currentLabel.c_str())) {
		int id = 0;
		for (const T& value : choices) {
			ImGui::PushID(id++);
			std::string valueLabel = labelGenerator(value);
			bool isSelected = valueLabel == currentLabel;
			if (ImGui::Selectable(valueLabel.c_str(), isSelected)) {
				*current = value;
				changed = true;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return changed;
}
