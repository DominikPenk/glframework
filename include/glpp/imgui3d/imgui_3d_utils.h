#pragma once

#include <glm/glm.hpp>

#include "glpp/imgui3d/imgui_3d.h"

namespace ImGui3D {

	std::pair<glm::vec4, glm::vec4> worldCameraRay(ImVec2 mouse);

	ImVec2 toClipSpace(ImVec2 q);

	static inline glm::vec3 toClipSpace(const glm::mat4& MVP, const glm::vec4 p) {
		const glm::vec4 q = MVP * p;
		return glm::vec3(q) / q.w;
	}
}