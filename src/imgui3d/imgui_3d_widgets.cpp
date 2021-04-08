#include "glpp/imgui3d/imgui_3d.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "glpp/imgui.hpp"

#include "glpp/imgui3d/imgui_3d_geometry.h"
#include "glpp/imgui3d/imgui_3d_utils.h"

namespace ImGui3D {

	bool RotationGizmo(const float _pos[3], float _quat[4], glm::mat3 coordinateSystem, ImGuiID idoverride)
	{
		// Rotation gizmo computes the delta angle
		static glm::vec4 s_startPoint;
		static glm::vec3 s_lastPoint;
		static float s_angle;

		ImGui3DContext& g = *GImGui3D;

		glm::vec3 pos(_pos[0], _pos[1], _pos[2]);

		// Scale gizmo based on distance
		const float s = g.Style.GizmoSize * glm::distance(g.ViewMatrix * glm::vec4(pos, 1), glm::vec4(0, 0, 0, 1)) / g.ScreenSize.x;

		// Draw circles
		glm::mat4 T(coordinateSystem);
		T[3] = glm::vec4(_pos[0], _pos[1], _pos[2], 1.0f);

		int segments = g.Style.RotationGizmoSegments;
		const float delta_angle = glm::two_pi<float>() / segments;
		std::vector<glm::vec4> line_points;
		for (int i = 0; i < segments; ++i) {
			const float angle = delta_angle * i;
			line_points.push_back(glm::vec4(s * std::cos(angle), s * std::sin(angle), 0, 1));
		}

		if (idoverride == 0)
			ImGui3D::PushID(_quat);
		else
			ImGui3D::PushID(idoverride);


		ImGuiIO& io = ImGui::GetIO();
		bool retVal = false;

		// Check each axis for input
		for (int axis = 0; axis < 3; ++axis) {
			const ImGuiID id = ImGui3D::GetID("axis" + axis);
			const glm::vec4 col = g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis));
			g.currentDrawList()->AddPolyLine(line_points, col, 5, true, T * orthonormalToWorld(getAxis4(axis)), id);

			
			if (io.MouseClicked[0] && ImGui3D::IsItemActive()) {
				// First mouse click
				auto [o, d] = worldCameraRay(io.MousePos);
				s_startPoint = closestPointOnUnitCircle(o, d, glm::vec4(pos, 1), glm::vec4(coordinateSystem[axis], 0.f));
				s_lastPoint = s_startPoint;
			}
			else if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
				// Get current angle
				auto [o, d] = worldCameraRay(io.MousePos);
				glm::vec3 n = coordinateSystem[axis];
				glm::vec4 currentPoint = closestPointOnUnitCircle(o, d, glm::vec4(pos, 1), glm::vec4(n, 0.f));
				retVal = true;

				// Visualize angle
				float composedAngle = glm::orientedAngle(
					glm::normalize(glm::vec3(s_startPoint) - pos), 
					glm::normalize(glm::vec3(currentPoint) - pos), 
					n);
				glm::quat quat = glm::angleAxis(composedAngle, n);
				std::memcpy(_quat, &quat.x, sizeof(quat));

				g.currentDrawList()->AddFilledSemiCircle(
					pos, 
					s * glm::vec3(s_startPoint - glm::vec4(pos, 1)), 
					n, composedAngle,
					g.Style.Colors[ImGui3DCol_xRotation_circle + axis], 24);

				s_lastPoint = currentPoint;
			}
		}
		ImGui3D::PopID();
		return retVal;
	}
	
	bool RotationGizmo(glm::mat4& T, glm::mat3 coordinateSystem)
	{
		static bool s_rotation_active = false;
		static glm::mat4 s_Told;

		ImGuiID rotId = GetID(&T, false);
		glm::quat q;
		bool rUpdating = RotationGizmo(&T[3].x, &q.x, coordinateSystem, rotId);

		if (rUpdating) {
			if (!s_rotation_active) {
				// This is the first time the rotation is updated
				s_Told = T;
				s_rotation_active = true;
			}
			T = glm::toMat4(q) * s_Told;
		}
		else
		{
			s_rotation_active = false;
		}
		return rUpdating;
	}

	bool RotationGizmo(glm::mat4& T, bool local)
	{
		static bool s_rotation_active = false;
		static glm::mat4 s_Told;

		ImGuiID rotId = GetID(&T, false);
		glm::quat q;
		glm::mat3 coo = local
			? s_rotation_active ? s_Told : T
			: glm::mat3(1);
		bool rUpdating = RotationGizmo(&T[3].x, &q.x, coo, rotId);

		if (rUpdating) {
			if (!s_rotation_active) {
				// This is the first time the rotation is updated
				s_Told = T;
				s_rotation_active = true;
			}
			T = glm::toMat4(q) * s_Told;
		}
		else
		{
			s_rotation_active = false;
		}
		return rUpdating;
	}

	bool RotationGizmo(glm::vec3 pos, glm::quat& quat, glm::mat3 coordinateSystem)
	{
		return RotationGizmo(&pos[0], &quat[0], coordinateSystem);
	}

	bool TranslationGizmo(float _pos[3], glm::mat3 coordinateSystem) {
		// Scale gizmo based on distance
		ImGui3DContext& g = *GImGui3D;
		const float distance = glm::distance(g.ViewMatrix * glm::vec4(_pos[0], _pos[1], _pos[2], 1), glm::vec4(0, 0, 0, 1));
		const float s = g.Style.GizmoSize * distance / g.ScreenSize.x;
		const float planesOffset = s * g.Style.TranslationGizmoInnerPadding;
		const float planesSize = s * g.Style.TranslationGizmoPlaneSize;

		ImGui3D::PushID(_pos);

		glm::mat4 T(coordinateSystem);
		T[3] = glm::vec4(_pos[0], _pos[1], _pos[2], 1.0f);

		ImGuiIO& io = ImGui::GetIO();
		bool retVal = false;
		for (int axis = 0; axis < 3; ++axis) {
			// Arrows
			const ImGuiID id = ImGui3D::GetID("axis" + axis);

			g.currentDrawList()->AddArrow(
				T[3], T * glm::vec4(s * getAxis3(axis), 1),
				g.Style.getColor(static_cast<ImGui3DColors>(ImGui3DCol_xAxis + 2 * axis)), 
				.03 * s, .11 * s, 
				.35f, 16, id);
			if (ImGui::IsMouseDragging(0) && ImGui3D::IsItemActive()) {
				const ImVec2 MousePosPrev = io.MousePos - io.MouseDelta;
				auto [oprev, dprev] = worldCameraRay(MousePosPrev);
				auto [ocurr, dcurr] = worldCameraRay(io.MousePos);
				glm::vec4 dir = glm::vec4(coordinateSystem * getAxis3(axis), 0.f);
				glm::vec4 pprev = getClosestPointOnLine(glm::vec4(_pos[0], _pos[1], _pos[2], 1), dir, oprev, dprev);
				glm::vec4 pcurr = getClosestPointOnLine(glm::vec4(_pos[0], _pos[1], _pos[2], 1), dir, ocurr, dcurr);
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
			const glm::vec3 uAxis = coordinateSystem * getAxis3((axis + 1) % 3);
			const glm::vec3 vAxis = coordinateSystem * getAxis3((axis + 2) % 3);
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
				glm::vec4 normal = glm::vec4(coordinateSystem * getAxis3(axis), 0.f);
				glm::vec4 pprev = pointPlaneIntersection(oprev, dprev, glm::vec4(_pos[0], _pos[1], _pos[2], 1), normal);
				glm::vec4 pcurr = pointPlaneIntersection(ocurr, dcurr, glm::vec4(_pos[0], _pos[1], _pos[2], 1), normal);
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
	bool TranslationGizmo(glm::vec4& pos, glm::mat3 coordinateSystem)
	{
		return TranslationGizmo(&pos[0]);
	}
	bool TranslationGizmo(glm::vec3& pos, glm::mat3 coordinateSystem)
	{
		return TranslationGizmo(&pos[0]);
	}

	bool TransformGizmo(float _T[16], bool local)
	{
		static bool s_rotation_active = false;
		static glm::mat4 s_Told;

		ImGuiID rotId = GetID(_T, false);
		glm::quat q;
		glm::mat3 coo = local 
			? s_rotation_active ? s_Told : glm::make_mat4(_T)
			: glm::mat3(1);
		bool rUpdating = RotationGizmo(&_T[4 * 3], &q[0], coo, rotId);
		bool tUpdating = TranslationGizmo(&_T[4 * 3], coo);

		if (rUpdating) {
			if (!s_rotation_active) {
				// This is the first time the rotation is updated
				s_Told = glm::make_mat4(_T);
				s_rotation_active = true;
			}
			glm::mat4 T = glm::toMat4(q) * s_Told;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					_T[4 * i + j] = T[i][j];
				}
			}
			//std::printf("Angle: %.3f %.3f %.3f\r", angles[0], angles[1], angles[2]);
		}
		else
		{
			s_rotation_active = false;
		}
		return rUpdating || tUpdating;
	}
	bool TransformGizmo(glm::mat4& T, bool local)
	{
		return TransformGizmo(&T[0][0], local);
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