#include "glpp/imgui3d/imgui_3d_utils.h"

std::pair<glm::vec4, glm::vec4> ImGui3D::worldCameraRay(ImVec2 mouse)
{
	const ImGui3DContext& g = *GImGui3D;

	mouse = mouse - g.ScreenPosition;

	float ndcX = 2.0f * mouse.x / g.ScreenSize.x - 1.0f;
	float ndcY = -(2.0f * mouse.y / g.ScreenSize.y - 1.0f);
	glm::vec4 clip(ndcX, ndcY, -1.0f, 1.0f);
	glm::vec4 eye = glm::inverse(g.ProjectionMatrix) * clip;
	eye.z = -1.f;
	eye.w = 0.f;
	glm::mat4 invV = glm::inverse(g.ViewMatrix);
	return std::make_pair(
		invV * glm::vec4(0, 0, 0, 1),
		invV * eye
	);
}

ImVec2 ImGui3D::toClipSpace(ImVec2 q)
{
	const ImGui3DContext& g = *GImGui3D;
	return ImVec2(2.0f * q.x / g.ScreenSize.x - 1.0f, -(2.0f * q.y / g.ScreenSize.y - 1.0f));
}
