#pragma once

#include "glpp/imgui3d/imgui_3d.h"

namespace ImGui3D {

	bool PointLight(float position[3]);
	bool PointLight(glm::vec3& pos);
	bool PointLight(glm::vec4& pos);

	bool DirectionalLight(float direction[3], float position[3]);
	bool DirectionalLight(glm::vec3& direction, glm::vec3& position);
	bool DirectionalLight(glm::vec4& direction , glm::vec4& position);

	bool Spotlight(float direction[3], float position[3], float* angle);
	bool Spotlight(glm::vec3& direction, glm::vec3& position, float& angle);
	bool Spotlight(glm::vec4& direction, glm::vec4& position, float& angle);

	bool CamerViewDirection(float ViewMatrix[16]);
	bool CamerViewDirection(glm::mat4& ViewMatrix);

	bool CubeMap(float position[3]);
	bool CubeMap(glm::vec3& position);
	bool CubeMap(glm::vec4& position);

}