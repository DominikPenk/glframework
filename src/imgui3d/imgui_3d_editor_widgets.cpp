#include "glpp/imgui3d/imgui_3d_editor_widgets.h"

#include "glpp/imgui.hpp"

#include "glpp/imgui3d/imgui_3d_geometry.h"
#include "glpp/imgui3d/imgui_3d_utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

void ImGui3D::ViewFrustum(float VP[16], glm::vec3 color)
{
	ViewFrustum(glm::make_mat4(VP), color);
}

void ImGui3D::ViewFrustum(glm::mat4& VP, glm::vec3 color)
{
	ImGui3DContext& g = *GImGui3D;

	glm::vec3 points[] = {
		{ -1.f, -1.f,  1.f }, // 0
		{  1.f, -1.f,  1.f }, // 1
		{ -1.f,  1.f,  1.f }, // 2
		{  1.f,  1.f,  1.f }, // 3
		{ -1.f, -1.f, -1.f }, // 4
		{  1.f, -1.f, -1.f }, // 5
		{ -1.f,  1.f, -1.f }, // 6
		{  1.f,  1.f, -1.f }, // 7
	};

	glm::mat4 invVP = glm::inverse(VP);
	for (int i = 0; i < 8; ++i) {
		glm::vec3 cube = points[i] * 0.5f - glm::vec3(0.5f);
		glm::vec3 ndc = cube * 2.0f - glm::vec3(1.0f);
		glm::vec4 homPosition = invVP * glm::vec4(ndc, 1.0f);
		points[i] = glm::vec3(homPosition) / homPosition.w;
	}

	const glm::vec4 col(color, 1.0);

	g.currentDrawList()->AddLine(points[0], points[1], 2, col);
	g.currentDrawList()->AddLine(points[0], points[2], 2, col);
	g.currentDrawList()->AddLine(points[1], points[3], 2, col);
	g.currentDrawList()->AddLine(points[2], points[3], 2, col);
	g.currentDrawList()->AddLine(points[4], points[5], 2, col);
	g.currentDrawList()->AddLine(points[4], points[6], 2, col);
	g.currentDrawList()->AddLine(points[5], points[7], 2, col);
	g.currentDrawList()->AddLine(points[6], points[7], 2, col);
	g.currentDrawList()->AddLine(points[0], points[4], 2, col);
	g.currentDrawList()->AddLine(points[1], points[5], 2, col);
	g.currentDrawList()->AddLine(points[2], points[6], 2, col);
	g.currentDrawList()->AddLine(points[3], points[7], 2, col);
}

bool ImGui3D::PointLight(float p[3])
{
	ImGui3DContext& g = *GImGui3D;

	const ImGuiID id = ImGui3D::GetID(p);
	const glm::vec4 col = g.Style.getColor(ImGui3DCol_Editor_Widget);

	glm::vec4 pos = glm::vec4(p[0], p[1], p[2], 1.f);
	g.currentDrawList()->AddDashedCircle(pos, g.Style.LightRadius, 1, col, 16, 2, id);
	g.currentDrawList()->AddScreenAlignedCircle(pos, .7 * g.Style.LightRadius, 1, col, 24, id);
	g.currentDrawList()->AddFilledScreenAlignedCircle(pos, .25f * g.Style.LightRadius, col, 24, id);
	g.currentDrawList()->AddFilledScreenAlignedCircle(pos, g.Style.LightRadius, glm::vec4(0, 0, 0, 0), 16, id);

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
		const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
		const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
		const auto [ocenter, dcenter] = worldCameraRay(g.ScreenSize / 2);
		glm::vec4 pcurr = pointPlaneIntersection(ocurr, dcurr, glm::vec4(0, 0, 0, 1), dcenter);
		if (std::isfinite(pcurr.x)) {
			p[0] = pcurr.x;
			p[1] = pcurr.y;
			p[2] = pcurr.z;
			return true;
		}
	}
	return false;
}

bool ImGui3D::PointLight(glm::vec3& pos)
{
	return PointLight(&pos[0]);
}

bool ImGui3D::PointLight(glm::vec4& pos)
{
	return PointLight(&pos[0]);
}

bool ImGui3D::DirectionalLight(float _d[3], float _p[3])
{
	ImGui3DContext& g = *GImGui3D;
	ImGuiIO& io = ImGui::GetIO();

	bool retVal = PointLight(_p);
	glm::vec4 col = g.Style.getColor(ImGui3DCol_Editor_Widget);

	glm::vec3 viewDir = glm::vec3(glm::inverse(g.ViewMatrix) * glm::vec4(0, 0, -1, 0));

	glm::vec3 dir(_d[0], _d[1], _d[2]);
	glm::vec3 pos(_p[0], _p[1], _p[2]);
	const float scale = glm::distance(g.ViewMatrix * glm::vec4(pos, 1), glm::vec4(0, 0, 0, 1)) / g.ScreenSize.x;

	// Orientation
	const glm::vec3 end = pos + scale * g.Style.LightDirectionLength * dir;
	g.currentDrawList()->AddLine(pos, end, 1, col);

	const ImGuiID dirId = ImGui3D::GetID(_d);
	const glm::vec4 dirCol = g.Style.getColor(ImGui3DCol_Editor_Widget);
	g.currentDrawList()->AddFilledScreenAlignedCircle(end - 0.1f * viewDir, g.Style.LightHandleSize, dirCol, 24, dirId);
	if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
		const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
		const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
		glm::vec4 pcurr = closestPointOnUnitCircle(ocurr, dcurr, glm::vec4(pos, 1), glm::vec4(viewDir, 0));
		if (std::isfinite(pcurr.x)) {
			_d[0] = pcurr.x - pos.x;
			_d[1] = pcurr.y - pos.y;
			_d[2] = pcurr.z - pos.z;
			retVal = true;
		}
	}
	return retVal;
}

bool ImGui3D::DirectionalLight(glm::vec3& direction, glm::vec3& position)
{
	return DirectionalLight(&direction[0], &position[0]);
}

bool ImGui3D::DirectionalLight(glm::vec4& direction, glm::vec4& position)
{
	return DirectionalLight(&direction[0], &position[0]);
}

bool ImGui3D::Spotlight(float d[3], float p[3], float* angle)
{
	ImGui3DContext& g = *GImGui3D;
	ImGuiIO& io = ImGui::GetIO();

	bool retval = DirectionalLight(d, p);
	const glm::vec4 col = g.Style.getColorForID(ImGui3D::GetID(p), ImGui3DCol_Editor_Widget);
	
	glm::vec3 viewDir = glm::vec3(glm::inverse(g.ViewMatrix) * glm::vec4(0, 0, -1, 0));
	const glm::vec3 pos(p[0], p[1], p[2]);
	const glm::vec3 dir(d[0], d[1], d[2]);
	
	// Opening angle
	const float scale = glm::distance(g.ViewMatrix * glm::vec4(pos, 1), glm::vec4(0, 0, 0, 1)) / g.ScreenSize.x;
	const float halfAngle = 0.5f * *angle;
	const float height = g.Style.SpotLightConeHeight * scale;

	const float r = std::tan(halfAngle) * height;
	glm::vec3 a = glm::normalize(glm::cross(dir, viewDir));
	glm::vec3 p0 = pos + height * dir + a * r;
	glm::vec3 p1 = pos + height * dir - a * r;

	g.currentDrawList()->AddLine(glm::vec4(pos, 1), glm::vec4(p0, 1), 1, col);
	g.currentDrawList()->AddLine(glm::vec4(pos, 1), glm::vec4(p1, 1), 1, col);
	g.currentDrawList()->AddCircle(pos + height * dir, dir, r, 2, col, 32);
	
	auto mapRange = [](float t, float in_min, float in_max, float out_min, float out_max) {
		float a = std::clamp((t - in_min) / (in_max - in_min), 0.f, 1.f);
		return out_min + a * (out_max - out_min);
	};
	const float t = mapRange(glm::degrees(*angle), 1.f, 179.f, .1f * height, .8f * height);
	const glm::vec4 p4 = glm::vec4(pos + t * dir - 0.01f * viewDir, 1);
	const ImGuiID angleId = ImGui3D::GetID(angle);
	const glm::vec4 angleCol = g.Style.getColor(ImGui3DCol_Editor_Widget);
	g.currentDrawList()->AddFilledScreenAlignedQuad(p4, ImVec2(12, 12), angleCol, angleId);
	if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
		const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
		const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
		glm::vec4 pcurr = getClosestPointOnLine(glm::vec4(pos, 1), glm::vec4(dir, 0), ocurr, dcurr);
		if (std::isfinite(pcurr.x)) {
			*angle = mapRange(glm::distance(glm::vec3(pcurr), pos), 0.1f * height, .8f * height, glm::radians(1.0f), glm::radians(179.f));
			retval = true;
		}
	}
	return retval;
}

bool ImGui3D::Spotlight(glm::vec3& direction, glm::vec3& position, float& angle)
{
	return Spotlight(&direction[0], &position[0], &angle);
}

bool ImGui3D::Spotlight(glm::vec4& direction, glm::vec4& position, float& angle)
{
	return Spotlight(&direction[0], &position[0], &angle);
}

bool ImGui3D::CamerViewDirection(float _ViewMatrix[16])
{

	ImGui3DContext& g = *GImGui3D;

	glm::mat4 ViewMatrix = glm::make_mat4(_ViewMatrix);
	glm::mat4 ViewProjectionMatrix = g.ProjectionMatrix * ViewMatrix;

	auto GetCameraMatrix = [&ViewMatrix](glm::vec3 axis) {
		glm::vec4 pos = ViewMatrix * glm::vec4(0, 0, 0, 1);
		float r = std::sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
		if (axis.y == 1 || axis.y == -1) {
			return glm::lookAt(r * axis, glm::vec3(0), glm::vec3(0, 0, 1));
		}
		return glm::lookAt(r * axis, glm::vec3(0), glm::vec3(0, 1, 0));
	};
	ImVec2 center = ImVec2(g.ScreenSize.x - 60, 60);

	ImGui3D::PushID(&g.ViewMatrix[0]);
	g.currentDrawList()->AddFilledScreenSpaceCircle(center, 50, glm::vec4(.75, .75, .75, .25), 64, .5f);

	// Project the coordinate origin to screen space
	for (int axis = 0; axis < 3; ++axis) {
		glm::vec4 d = ViewProjectionMatrix * getAxis4(axis);
		ImVec2 screenSpaceDir(d.x / d.z / d.w, -d.y / d.z / d.w);
		const float length = std::sqrt(screenSpaceDir.x * screenSpaceDir.x + screenSpaceDir.y * screenSpaceDir.y);
		if (length > 1.0f) {
			screenSpaceDir /= length;
		}
		screenSpaceDir *= 40.0f;
		ImGuiID idForward = ImGui3D::GetID("axis" + axis, false);
		glm::vec4 colForward = g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis));
		g.currentDrawList()->AddScreenSpaceLine(center, center + screenSpaceDir, 3, colForward, .1f + axis / 100.f, idForward);
		g.currentDrawList()->AddFilledScreenSpaceCircle(center + screenSpaceDir, 8, colForward, 18, (d.z < 0 ? .0f : .2f) + axis / 100.f, idForward);
		if (IsItemClicked()) {
			glm::mat4 V = GetCameraMatrix(getAxis3(axis));
			std::copy(glm::value_ptr(V), glm::value_ptr(V) + 16, _ViewMatrix);
		}


		ImGuiID idBackward = ImGui3D::GetID("axis_backward" + axis, false);
		glm::vec4 colBackward = g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis));
		g.currentDrawList()->AddFilledScreenSpaceCircle(center - screenSpaceDir, 8, colBackward, 18, d.z < 0 ? .0f : .2f + axis / 100.f, idBackward);
		if (IsItemClicked()) {
			glm::mat4 V = GetCameraMatrix(-getAxis3(axis));
			std::copy(glm::value_ptr(V), glm::value_ptr(V) + 16, _ViewMatrix);
		}
	}


	return false;
}

bool ImGui3D::CamerViewDirection(glm::mat4& ViewMatrix)
{
	return CamerViewDirection(glm::value_ptr(ViewMatrix));
}

bool ImGui3D::CubeMap(float p[3])
{
	ImGui3DContext& g = *GImGui3D;

	const ImGuiID id = ImGui3D::GetID(p);
	const glm::vec4 col = g.Style.getColor(ImGui3DCol_Editor_Widget);
	const glm::vec4 pos(p[0], p[1], p[2], 1);

	const float scale = glm::distance(g.ViewMatrix * pos, glm::vec4(0, 0, 0, 1)) / g.ScreenSize.x;

	g.currentDrawList()->AddAxisAlignedCube(      pos, scale * glm::vec3(g.Style.CubeMapSize), 1, col, id);
	g.currentDrawList()->AddFilledAxisAlignedCube(pos, scale * glm::vec3(g.Style.CubeMapSize), glm::vec4(0), id);
	g.currentDrawList()->AddCircle(pos, getAxis4(0), g.Style.CubeMapRadius * scale, 1, col, 24, 0);
	g.currentDrawList()->AddCircle(pos, getAxis4(1), g.Style.CubeMapRadius * scale, 1, col, 24, 0);
	g.currentDrawList()->AddCircle(pos, getAxis4(2), g.Style.CubeMapRadius * scale, 1, col, 24, 0);

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
		const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
		const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
		const auto [ocenter, dcenter] = worldCameraRay(g.ScreenSize / 2);
		glm::vec4 pcurr = pointPlaneIntersection(ocurr, dcurr, glm::vec4(0, 0, 0, 1), dcenter);
		if (std::isfinite(pcurr.x)) {
			p[0] = pcurr.x;
			p[1] = pcurr.y;
			p[2] = pcurr.z;
			return true;
		}
	}
	return false;
}

bool ImGui3D::CubeMap(glm::vec3& position)
{
	return CubeMap(&position[0]);
}

bool ImGui3D::CubeMap(glm::vec4& position)
{
	return CubeMap(&position[0]);
}
