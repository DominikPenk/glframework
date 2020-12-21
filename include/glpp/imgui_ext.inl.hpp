
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