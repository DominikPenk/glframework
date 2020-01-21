#include "imgui3d/imgui_3d.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "imgui.h"
#include "imgui_internal.h"

#include "imgui3d/imgui_3d_geometry.h"
#include "imgui3d/imgui_3d_utils.h"

namespace ImGui3D {

	bool RotationGizmo(const float _pos[3], float _angles[3], ImGuiID idoverride)
	{
		static glm::vec3 s_origAngles;
		static glm::vec4 s_startPoint;
		static float s_angle;

		ImGui3DContext& g = *GImGui3D;

		glm::vec3 pos(_pos[0], _pos[1], _pos[2]);

		// Scale gizmo based on distance
		const float s = g.Style.GizmoSize * glm::distance(g.ViewMatrix * glm::vec4(pos, 1), glm::vec4(0, 0, 0, 1)) / g.ScreenSize.x;

		const glm::mat4 T = glm::translate(pos);
		int segments = g.Style.RotationGizmoSegments;
		const float delta_angle = glm::two_pi<float>() / segments;
		std::vector<glm::vec4> line_points;
		for (int i = 0; i < segments; ++i) {
			const float angle = delta_angle * i;
			line_points.push_back(glm::vec4(s * std::cos(angle), s * std::sin(angle), 0, 1));
		}

		if (idoverride == 0)
			ImGui3D::PushID(_angles);
		else
			ImGui3D::PushID(idoverride);


		ImGuiIO& io = ImGui::GetIO();
		bool retVal = false;
		for (int axis = 0; axis < 3; ++axis) {
			const ImGuiID id = ImGui3D::GetID("axis" + axis);
			const glm::vec4 col = g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis));
			g.currentDrawList()->AddPolyLine(line_points, col, 5, true, T * orthonormalToWorld(getAxis4(axis)), id);

			
			if (io.MouseClicked[0] && ImGui3D::IsItemActive()) {
				auto [o, d] = worldCameraRay(io.MousePos);
				s_startPoint = closestPointOnUnitCircle(o, d, glm::vec4(pos, 1), getAxis4(axis));
				s_origAngles = glm::vec3(_angles[0], _angles[1], _angles[2]);
				s_angle = 0;
			}
			else if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
				// Get current angle
				auto [o, d] = worldCameraRay(io.MousePos);
				glm::vec4 currentPoint = closestPointOnUnitCircle(o, d, glm::vec4(pos, 1), getAxis4(axis));
				float angle = glm::orientedAngle(glm::vec3(s_startPoint) - pos, glm::vec3(currentPoint) - pos, getAxis3(axis));

				// Check if we can "fix" the angle
				const float delta_angle = std::abs(s_angle - angle);
				if (std::abs(s_angle - angle - glm::two_pi<float>()) < delta_angle) {
					angle += glm::two_pi<float>();
				}
				else if (std::abs(s_angle - angle + glm::two_pi<float>()) < delta_angle) {
					angle -= glm::two_pi<float>();
				}
				if (angle > glm::two_pi<float>()) {
					angle -= glm::two_pi<float>();
				}
				if (angle < -glm::two_pi<float>()) {
					angle += glm::two_pi<float>();
				}
				_angles[axis] = s_origAngles[axis] + angle;
				retVal = true;

				g.currentDrawList()->AddFilledSemiCircle(
					pos, 
					s * glm::vec3(s_startPoint - glm::vec4(pos, 1)), 
					getAxis3(axis), s_angle,
					g.Style.Colors[ImGui3DCol_xRotation_circle + axis], 24);
			}
		}
		ImGui3D::PopID();
		return retVal;
	}
	bool RotationGizmo(glm::vec4 pos, glm::vec3& angles)
	{
		return RotationGizmo(&pos[0], &angles[0]);
	}
	bool RotationGizmo(glm::vec3 pos, glm::vec3& angles)
	{
		return RotationGizmo(&pos[0], &angles[0]);
	}

	bool TranslationGizmo(float _pos[3]) {
		// Scale gizmo based on distance
		ImGui3DContext& g = *GImGui3D;
		const float distance = glm::distance(g.ViewMatrix * glm::vec4(_pos[0], _pos[1], _pos[2], 1), glm::vec4(0, 0, 0, 1));
		const float s = g.Style.GizmoSize * distance / g.ScreenSize.x;
		const float planesOffset = s * g.Style.TranslationGizmoInnerPadding;
		const float planesSize = s * g.Style.TranslationGizmoPlaneSize;

		ImGui3D::PushID(_pos);

		const glm::mat4 T = glm::translate(glm::vec3(_pos[0], _pos[1], _pos[2]));


		ImGuiIO& io = ImGui::GetIO();
		bool retVal = false;
		for (int axis = 0; axis < 3; ++axis) {
			// Arrows
			const ImGuiID id = ImGui3D::GetID("axis" + axis);
			g.currentDrawList()->AddArrow(
				T * glm::vec4(0, 0, 0, 1), T * glm::vec4(s * getAxis3(axis), 1),
				g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis)), 
				.03 * s, .11 * s, 
				.35f, 16, id);
			if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
				const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
				auto [oprev, dprev] = worldCameraRay(MousePosPrev);
				auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
				glm::vec4 pprev = getClosestPointOnLine(glm::vec4(0, 0, 0, 1), getAxis4(axis), oprev, dprev);
				glm::vec4 pcurr = getClosestPointOnLine(glm::vec4(0, 0, 0, 1), getAxis4(axis), ocurr, dcurr);
				if (std::isfinite(pprev.x) && std::isfinite(pcurr.x)) {
					glm::vec delta = glm::vec3(pcurr - pprev);
					_pos[0] += delta.x;
					_pos[1] += delta.y;
					_pos[2] += delta.z;
					retVal = true;
				}
			}

			// Plane
			const ImGuiID idPlane = ImGui3D::GetID("plane" + axis);
			const glm::vec3 uAxis = getAxis3((axis + 1) % 3);
			const glm::vec3 vAxis = getAxis3((axis + 2) % 3);
			const glm::vec3 offset = planesOffset * (uAxis + vAxis);
			g.currentDrawList()->AddTriangleStrip(std::vector<glm::vec4>{
				glm::vec4(offset, 1),
				glm::vec4(offset + planesSize * uAxis, 1),
				glm::vec4(offset + planesSize * vAxis, 1),
				glm::vec4(offset + planesSize * (uAxis + vAxis), 1) },
				g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_yzPlane + 2 * axis)),
				T, idPlane);
			if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
				const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
				auto [oprev, dprev] = worldCameraRay(MousePosPrev);
				auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
				glm::vec4 pprev = pointPlaneIntersection(oprev, dprev, glm::vec4(0, 0, 0, 1), getAxis4(axis));
				glm::vec4 pcurr = pointPlaneIntersection(ocurr, dcurr, glm::vec4(0, 0, 0, 1), getAxis4(axis));
				if (std::isfinite(pprev.x) && std::isfinite(pcurr.x)) {
					glm::vec delta = glm::vec3(pcurr - pprev);
					_pos[0] += delta.x;
					_pos[1] += delta.y;
					_pos[2] += delta.z;
					retVal = true;
				}
			}
		}

		ImGui3D::PopID();
		return retVal;
	}
	bool TranslationGizmo(glm::vec4& pos)
	{
		return TranslationGizmo(&pos[0]);
	}
	bool TranslationGizmo(glm::vec3& pos)
	{
		return TranslationGizmo(&pos[0]);
	}

	bool TransformGizmo(float _T[16])
	{
		float angles[3];
		glm::extractEulerAngleXYZ(glm::make_mat4(_T), angles[0], angles[1], angles[2]);

		ImGuiID rotId = GetID(_T, false);
		bool retVal = RotationGizmo(&_T[4 * 3], angles, rotId);
		retVal |= TranslationGizmo(&_T[4 * 3]);

		if (retVal) {
			glm::mat4 T = glm::eulerAngleXYZ(angles[0], angles[1], angles[2]);
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					_T[4 * i + j] = T[i][j];
				}
			}
		}
		return retVal;
	}
	bool TransformGizmo(glm::mat4& T)
	{
		return TransformGizmo(&T[0][0]);
	}

	bool Vertex(float pos[3]) {
		ImGui3DContext& g = *GImGui3D;

		ImGuiID id = ImGui3D::GetID(pos);
		g.currentDrawList()->AddFilledScreenAlignedQuad(glm::vec4(pos[0], pos[1], pos[2], 1), ImVec2(g.Style.VertexSize, g.Style.VertexSize), g.Style.getColorForID(id, ImGui3DCol_Vertex), id);

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
			const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
			const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
			const auto [ocenter, dcenter] = worldCameraRay(g.ScreenSize / 2);
			glm::vec4 pcurr = pointPlaneIntersection(ocurr, dcurr, glm::vec4(0, 0, 0, 1), dcenter);
			if (std::isfinite(pcurr.x)) {
				pos[0] = pcurr.x;
				pos[1] = pcurr.y;
				pos[2] = pcurr.z;
				return true;
			}
		}
		return false;
	}
	bool Vertex(glm::vec3& pos)
	{
		return Vertex(&pos[0]);
	}
	bool Vertex(glm::vec4& pos)
	{
		return Vertex(&pos[0]);
	}

	bool RestrictedVertex(float pos[3], const float axis[3])
	{
		ImGui3DContext& g = *GImGui3D;

		ImGuiID id = ImGui3D::GetID(pos);
		g.currentDrawList()->AddFilledScreenAlignedQuad(glm::vec4(pos[0], pos[1], pos[2], 1), ImVec2(g.Style.VertexSize, g.Style.VertexSize), g.Style.getColorForID(id, ImGui3DCol_Vertex), id);

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
			const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
			const auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
			glm::vec4 pcurr = getClosestPointOnLine(glm::vec4(pos[0], pos[1], pos[2], 1.0f), glm::vec4(axis[0], axis[1], axis[2], 0.f), ocurr, dcurr);
			if (std::isfinite(pcurr.x)) {
				pos[0] = pcurr.x;
				pos[1] = pcurr.y;
				pos[2] = pcurr.z;
				return true;
			}
		}
		return false;
	}
	bool RestrictedVertex(glm::vec3& pos, glm::vec3 axis)
	{
		return RestrictedVertex(&pos[0], &axis[0]);
	}
	bool RestrictedVertex(glm::vec4& pos, glm::vec4 axis)
	{
		return RestrictedVertex(&pos[0], &axis[0]);
	}

	bool RestrictedVertex(float pos[3], const float axis[3], const glm::mat4 T)
	{
		ImGui3D::PushID(pos);
		glm::vec4 p = T * glm::vec4(pos[0], pos[1], pos[2], 1);
		glm::vec4 a = T * glm::vec4(axis[0], axis[1], axis[2], 0);

		if (RestrictedVertex(&p[0], &a[0])) {
			p = glm::inverse(T) * p;
			pos[0] = p.x;
			pos[1] = p.y;
			pos[2] = p.z;
			return true;
		}
		ImGui3D::PopID();
		return false;
	}
	bool RestrictedVertex(glm::vec3& pos, glm::vec3 axis, glm::mat4 T)
	{
		ImGui3D::PushID(&pos);
		glm::vec4 p = T * glm::vec4(pos, 1);
		glm::vec4 a = T * glm::vec4(axis, 0);
		if (RestrictedVertex(&p[0], &a[0])) {
			pos = glm::vec3(glm::inverse(T) * p);
			return true;
		}
		ImGui3D::PopID();
		return false;
	}
	bool RestrictedVertex(glm::vec4& pos, glm::vec4 axis, glm::mat4 T)
	{
		ImGui3D::PushID(&pos);
		glm::vec4 p = T * pos;
		glm::vec4 a = T * axis;
		if (RestrictedVertex(&p[0], &a[0])) {
			pos = glm::inverse(T) * p;
			return true;
		}
		ImGui3D::PopID();
		return false;
	}
}