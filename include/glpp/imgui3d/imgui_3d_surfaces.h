#pragma once

#include <functional>

#include <glm/glm.hpp>

#include "imgui.h"

namespace ImGui3D {

	void ParametricSurface(std::function<glm::vec3(float, float)> S, glm::vec4 color, ImVec4 domain, ImVec2 gridSize, unsigned int subdivions);

}