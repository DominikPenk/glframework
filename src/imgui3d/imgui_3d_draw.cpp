#include "imgui3d/imgui_3d.h"

#include "imgui3d/imgui_3d_geometry.h"
#include "imgui3d/imgui_3d_utils.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>


#define IM3D_NORMALIZE2F_OVER_ZERO(VX,VY)                         { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = 1.0f / std::sqrt(d2); VX *= inv_len; VY *= inv_len; } }
#define IM3D_NORMALIZE2F_OVER_EPSILON_CLAMP(VX,VY,EPS,INVLENMAX)  { float d2 = VX*VX + VY*VY; if (d2 > EPS)  { float inv_len = 1.0f / std::sqrt(d2); if (inv_len > INVLENMAX) inv_len = INVLENMAX; VX *= inv_len; VY *= inv_len; } }

namespace ImGui3D {
	void DrawCommand::execute()
	{
		ImGui3DContext& g = *GImGui3D;
		batch.execute(shader, "VP", g.ViewProjectionMatrix);
	}

	DrawCommand::DrawCommand()
	{
		data = batch.addVertexAttributes<glm::vec4, glm::vec4, ImGuiID, glm::vec2>();
		shader = gl::Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "imgui3d.glsl");
	}

	void DrawCommand::AddFilledScreenAlignedQuad(glm::vec4 pos, ImVec2 size, glm::vec4 color, ImGuiID id)
	{
		ImGui3DContext& g = *GImGui3D;
		const glm::mat4& MVP = g.ViewProjectionMatrix * g.ModelMatrix;
		glm::vec4 c = MVP * pos;

		glm::vec3 q(c.x / c.w, c.y / c.w, c.z / c.w);

		glm::vec4 p0(q.x - size.x / g.ScreenSize.x, q.y - size.y / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p1(q.x - size.x / g.ScreenSize.x, q.y + size.y / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p2(q.x + size.x / g.ScreenSize.x, q.y - size.y / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p3(q.x + size.x / g.ScreenSize.x, q.y + size.y / g.ScreenSize.y, q.z, 1.f);

		AddTriangleStrip(std::vector<glm::vec4>{ p0, p1, p2, p3 }, color, g.ViewPerspectiveMatrixInverse, id);
	}
	void DrawCommand::AddFilledScreenAlignedQuad(glm::vec3 pos, ImVec2 size, glm::vec4 color, ImGuiID id)
	{
		AddFilledScreenAlignedQuad(glm::vec4(pos, 1), size, color, id);
	}

	void DrawCommand::AddScreenAlignedQuad(glm::vec4 pos, ImVec2 size, float width, glm::vec4 color, ImGuiID id)
	{
		if (width < 0) {
			AddFilledScreenAlignedQuad(pos, size, color, id);
			return;
		}

		ImGui3DContext& g = *GImGui3D;
		const glm::mat4& MVP = g.ViewProjectionMatrix * g.ModelMatrix;
		glm::vec4 c = MVP * pos;

		glm::vec3 q(c.x / c.w, c.y / c.w, c.z / c.w);

		glm::vec4 p0(q.x - 0.5f * size.y / g.ScreenSize.x, q.y - 0.5f * size.x / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p1(q.x - 0.5f * size.y / g.ScreenSize.x, q.y + 0.5f * size.x / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p2(q.x + 0.5f * size.y / g.ScreenSize.x, q.y + 0.5f * size.x / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 p3(q.x + 0.5f * size.y / g.ScreenSize.x, q.y - 0.5f * size.x / g.ScreenSize.y, q.z, 1.f);

		glm::vec4 q0(q.x - (.5f * size.y - width) / g.ScreenSize.x, q.y - (0.5f * size.x - width) / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 q1(q.x - (.5f * size.y - width) / g.ScreenSize.x, q.y + (0.5f * size.x - width) / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 q2(q.x + (.5f * size.y - width) / g.ScreenSize.x, q.y + (0.5f * size.x - width) / g.ScreenSize.y, q.z, 1.f);
		glm::vec4 q3(q.x + (.5f * size.y - width) / g.ScreenSize.x, q.y - (0.5f * size.x - width) / g.ScreenSize.y, q.z, 1.f);

		AddTriangleStrip(std::vector<glm::vec4>{ p0, q0, p1, q1, p2, q2, p3, q3, p0, q0 }, color, g.ViewPerspectiveMatrixInverse, id);
	}

	void DrawCommand::AddFilledScreenAlignedCircle(glm::vec4 pos, float radius, glm::vec4 color, unsigned int segments, ImGuiID id)
	{
		ImGui3DContext& g = *GImGui3D;
		const glm::mat4& MVP = g.ViewProjectionMatrix * g.ModelMatrix;
		glm::vec4 c = MVP * pos;

		glm::vec3 q(c.x / c.w, c.y / c.w, c.z / c.w);
		unsigned int cId = data->size();
		data->push_back(g.ViewPerspectiveMatrixInverse * glm::vec4(q, 1.f), color, id, glm::vec2(0));
		const float aliasRadius = 2.0f;
		glm::vec4 aliasColor(color.r, color.g, color.b, 0.0f);

		gl::IndexBuffer& indices = *batch.indexBuffer;
		const float delta = glm::two_pi<float>() / segments;

		const unsigned int i0 = cId + 1;
		for (int i = 0; i < (int)segments; ++i) {
			const float angle = i * delta;
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			const glm::vec4 p0(q.x +  radius                * c / g.ScreenSize.x, q.y +  radius                * s / g.ScreenSize.y, q.z, 1.f);
			const glm::vec4 p1(q.x + (radius + aliasRadius) * c / g.ScreenSize.x, q.y + (radius + aliasRadius) * s / g.ScreenSize.y, q.z, 1.f);
			data->push_back(g.ViewPerspectiveMatrixInverse * p0, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p1, aliasColor, id, glm::vec2(0));

			const unsigned int idx0 = i0 + 2 * i;
			const unsigned int idx1 = i0 + 2 * ((i + 1) % segments);
			indices.insert(indices.end(), {
				cId, idx0, idx1,
				idx0, idx0 + 1, idx1,
				idx0 + 1, idx1, idx1 + 1
			});
		}
	}
	void DrawCommand::AddFilledScreenAlignedCircle(glm::vec3 pos, float radius, glm::vec4 color, unsigned int segments, ImGuiID id)
	{
		AddFilledScreenAlignedCircle(glm::vec4(pos, 1), radius, color, segments, id);
	}

	void DrawCommand::AddScreenAlignedCircle(glm::vec4 pos, float radius, float width, glm::vec4 color, unsigned int segments, ImGuiID id)
	{
		if (width < 0) {
			AddFilledScreenAlignedCircle(pos, radius, color, segments, id);
			return;
		}

		ImGui3DContext& g = *GImGui3D;
		const glm::mat4& MVP = g.ViewProjectionMatrix * g.ModelMatrix;
		glm::vec4 c = MVP * pos;

		glm::vec3 q(c.x / c.w, c.y / c.w, c.z / c.w);
		const float aliasRadius = 2.0f;
		glm::vec4 aliasColor(color.r, color.g, color.b, 0.0f);

		gl::IndexBuffer& indices = *batch.indexBuffer;
		const float delta = glm::two_pi<float>() / segments;

		const float router = radius + 0.5f * width;
		const float rinner = radius - 0.5f * width;

		const unsigned int i0 = data->size();
		for (int i = 0; i < (int)segments; ++i) {
			const float angle = i * delta;
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			const glm::vec4 p0(q.x + router                 * c / g.ScreenSize.x, q.y + router                 * s / g.ScreenSize.y, q.z, 1.f);
			const glm::vec4 p1(q.x + (router + aliasRadius) * c / g.ScreenSize.x, q.y + (router + aliasRadius) * s / g.ScreenSize.y, q.z, 1.f);
			const glm::vec4 p2(q.x + rinner                 * c / g.ScreenSize.x, q.y + rinner                 * s / g.ScreenSize.y, q.z, 1.f);
			const glm::vec4 p3(q.x + (rinner - aliasRadius) * c / g.ScreenSize.x, q.y + (rinner - aliasRadius) * s / g.ScreenSize.y, q.z, 1.f);
			data->push_back(g.ViewPerspectiveMatrixInverse * p0, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p1, aliasColor, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p2, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p3, aliasColor, id, glm::vec2(0));

			const unsigned int idx0 = i0 + 4 * i;
			const unsigned int idx1 = i0 + 4 * ((i + 1) % segments);
			indices.insert(indices.end(), {
				idx0, idx0 + 1, idx1,
				idx0 + 1, idx1, idx1 + 1,
				idx0, idx0 + 2, idx1 + 2,
				idx0, idx1 + 2, idx1,
				idx0 + 2, idx0 + 3, idx1 + 3,
				idx0 + 2, idx1 + 3, idx1 + 2
				});
		}
	}
	void DrawCommand::AddScreenAlignedCircle(glm::vec3 pos, float radius, float width, glm::vec4 color, unsigned int segments, ImGuiID id)
	{
		AddScreenAlignedCircle(glm::vec4(pos, 1), radius, width, color, segments, id);
	}

	void DrawCommand::AddDashedCircle(glm::vec4 pos, float radius, float width, glm::vec4 color, unsigned int segments, unsigned int subdivision, ImGuiID id)
	{
		ImGui3DContext& g = *GImGui3D;
		glm::vec4 c = g.ViewProjectionMatrix * pos;

		glm::vec3 q(c.x / c.w, c.y / c.w, c.z / c.w);
		const float aliasRadius = 2.0f;
		glm::vec4 aliasColor(color.r, color.g, color.b, 0.0f);

		gl::IndexBuffer& indices = *batch.indexBuffer;

		const float router = radius + 0.5f * width;
		const float rinner = radius - 0.5f * width;

		const float arcAngle = glm::two_pi<float>() / segments;
		const float deltaAngle = arcAngle / (subdivision - 1);

		for (int sid = 0; sid < (int)segments; sid += 2) {
			const unsigned int i0 = data->size();
			for (int i = 0; i < (int)subdivision; ++i) {
				const float angle = sid * arcAngle + i * deltaAngle;
				const float c = std::cos(angle);
				const float s = std::sin(angle);
				const glm::vec4 p0(q.x + router * c / g.ScreenSize.x, q.y + router * s / g.ScreenSize.y, q.z, 1.f);
				const glm::vec4 p1(q.x + (router + aliasRadius) * c / g.ScreenSize.x, q.y + (router + aliasRadius) * s / g.ScreenSize.y, q.z, 1.f);
				const glm::vec4 p2(q.x + rinner * c / g.ScreenSize.x, q.y + rinner * s / g.ScreenSize.y, q.z, 1.f);
				const glm::vec4 p3(q.x + (rinner - aliasRadius) * c / g.ScreenSize.x, q.y + (rinner - aliasRadius) * s / g.ScreenSize.y, q.z, 1.f);
				data->push_back(g.ViewPerspectiveMatrixInverse * p0, color, id, glm::vec2(0));
				data->push_back(g.ViewPerspectiveMatrixInverse * p1, aliasColor, id, glm::vec2(0));
				data->push_back(g.ViewPerspectiveMatrixInverse * p2, color, id, glm::vec2(0));
				data->push_back(g.ViewPerspectiveMatrixInverse * p3, aliasColor, id, glm::vec2(0));

				if (i > 0) {
					const unsigned int idx0 = i0 + 4 * (i - 1);
					const unsigned int idx1 = i0 + 4 * i;
					indices.insert(indices.end(), {
						idx0, idx0 + 1, idx1,
						idx0 + 1, idx1, idx1 + 1,
						idx0, idx0 + 2, idx1 + 2,
						idx0, idx1 + 2, idx1,
						idx0 + 2, idx0 + 3, idx1 + 3,
						idx0 + 2, idx1 + 3, idx1 + 2
						});
				}
			}
		}

	}
	void DrawCommand::AddDashedCircle(glm::vec3 pos, float radius, float width, glm::vec4 color, unsigned int segments, unsigned int subdivisions, ImGuiID id)
	{
		AddDashedCircle(glm::vec4(pos, 1), radius, width, color, segments, subdivisions, id);
	}

	void DrawCommand::AddTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c, glm::vec4 color, ImGuiID id)
	{
		unsigned int i0 = data->size();
		gl::IndexBuffer& indices = *batch.indexBuffer;
		data->extend({
			{a, color, id, glm::vec2(0)},
			{b, color, id, glm::vec2(0)},
			{c, color, id, glm::vec2(0)},
			});
		indices.insert(indices.end(), { i0, i0 + 1, i0 + 2 });
	}

	void DrawCommand::AddTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color, ImGuiID id)
	{
		AddTriangle(glm::vec4(a, 1), glm::vec4(b, 1), glm::vec4(c, 1), color, id);
	}

	void DrawCommand::AddTriangleStrip(std::vector<glm::vec4> points, std::vector<glm::vec4> colors, glm::mat4 T, ImGuiID id)
	{
		assert(points.size() == colors.size());
		unsigned int i0 = data->size();
		gl::IndexBuffer& indices = *batch.indexBuffer;
		for (int i = 0; i < points.size(); ++i) {
			data->push_back(T * points[i], colors[i], id, glm::vec2(0));
			if (i > 1) {
				indices.insert(indices.end(), { i0 + i - 2, i0 + i - 1, i0 + i });
			}
		}
	}
	void DrawCommand::AddTriangleStrip(std::vector<glm::vec3> points, std::vector<glm::vec4> colors, glm::mat4 T, ImGuiID id)
	{
		assert(points.size() == colors.size());
		unsigned int i0 = data->size();
		gl::IndexBuffer& indices = *batch.indexBuffer;
		for (int i = 0; i < points.size(); ++i) {
			data->push_back(T * glm::vec4(points[i], 1), colors[i], id, glm::vec2(0));
			if (i > 1) {
				indices.insert(indices.end(), { i0 + i - 2, i0 + i - 1, i0 + i });
			}
		}
	}

	void DrawCommand::AddTriangleStrip(std::vector<glm::vec4> points, glm::vec4 color, glm::mat4 T, ImGuiID id)
	{
		AddTriangleStrip(points, std::vector<glm::vec4>(points.size(), color), T, id);
	}
	void DrawCommand::AddTriangleStrip(std::vector<glm::vec3> points, glm::vec4 color, glm::mat4 T, ImGuiID id)
	{
		AddTriangleStrip(points, std::vector<glm::vec4>(points.size(), color), T, id);
	}

	void DrawCommand::AddCylinder(glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, bool close_bottom, bool close_top, ImGuiID id)
	{
		AddCylinder(glm::vec3(from), glm::vec3(to), radius, color, segments, close_bottom, close_top, id);
	}
	void DrawCommand::AddCylinder(glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, bool cb, bool ct, ImGuiID id)
	{
		const glm::mat4 ModelMatrix = glm::translate(from) * glm::transpose(zAxistoWorld(glm::vec4(to - from, 1)));
		const float length = glm::distance(from, to);

		std::vector<glm::vec4> points(2 * segments);
		const float delta_angle = glm::two_pi<float>() / segments;
		
		const unsigned int icb = data->size();
		const unsigned int ict = cb ? icb + 1 : icb;
		if (cb) {
			assert(data->size() == icb);
			data->push_back(glm::vec4(0, 0, 0, 1), color, id, glm::vec2(0));
		}
		if (ct) {
			data->push_back(glm::vec4(0, 0, length, 1), color, id, glm::vec2(0));
		}

		gl::IndexBuffer& indices = *batch.indexBuffer;

		const unsigned int id0 = data->size();
		for (int i = 0; i < segments; ++i) {
			const float a = delta_angle * i;
			const float c = radius * std::cos(a);
			const float s = radius * std::sin(a);
			points[2 * i + 0] = glm::vec4(c, s, 0, 1);
			points[2 * i + 1] = glm::vec4(c, s, length, 1);
		
			const unsigned int idx2 = id0 + 2 * i;
			const unsigned int idx1 = i == points.size() - 1 ? id0 : idx2 - 2;
			if (i > 0 && cb) {
				indices.insert(indices.end(), { icb, idx2, idx1 });
			}
			if (i > 0 && ct) {
				indices.insert(indices.end(), { ict, idx2 + 1, idx1 + 1 });
			}
		}
		AddTriangleStrip(points, color, ModelMatrix, id);
		indices.insert(indices.end(), {
			(unsigned int)data->size() - 2, id0, id0 + 1,
			(unsigned int)data->size() - 2, id0 + 1, (unsigned int)data->size() - 1
			});
	}

	void DrawCommand::AddCone(glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, bool close, ImGuiID id)
	{
		AddCone(glm::vec3(from), glm::vec3(to), radius, color, segments, close, id);
	}
	void DrawCommand::AddCone(glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, bool close, ImGuiID id)
	{
		const glm::mat4 T = glm::translate(from) * zAxistoWorld(glm::vec4(to - from, 1));
		const float height = glm::distance(from, to);

		const unsigned int bottomId = data->size();
		if (close) {
			data->push_back(glm::vec4(from, 1), color, id, glm::vec2(0));
		}
		const unsigned int topId = data->size();
		data->push_back(glm::vec4(to, 1), color, id, glm::vec2(0));
		const float delta_angle = glm::two_pi<float>() / segments;

		gl::IndexBuffer& indices = *batch.indexBuffer;

		const unsigned int id0 = data->size();
		for (int i = 0; i < segments; ++i) {
			const float a = delta_angle * i;
			data->push_back(T * glm::vec4(radius * std::cos(a), radius * std::sin(a), 0, 1), color, id, glm::vec2(0));

			const unsigned int idx1 = id0 + i;
			const unsigned int idx2 = id0 + (i + 1) % segments;
			indices.insert(indices.end(), { idx1, idx2, topId });
			if (close) {
				indices.insert(indices.end(), { idx1, idx2, bottomId });
			}
		}
	}

	void DrawCommand::AddArrow(glm::vec4 from, glm::vec4 to, glm::vec4 color, float r1, float r2, float tip, int segments, ImGuiID id)
	{
		AddArrow(glm::vec3(from), glm::vec3(to), color, r1, r2, tip, segments, id);
	}
	void DrawCommand::AddArrow(glm::vec3 from, glm::vec3 to, glm::vec4 color, float r1, float r2, float tip, int segments, ImGuiID id)
	{
		float length = glm::distance(from, to);
		if (length < 1e-4f) return;
		glm::vec3 center = from + (1.0f - tip) * (to - from);
		
		AddCylinder(from, center, r1, color, segments, false, false, id);
		AddCone(center, to, r2, color, segments, true, id);
	}

	void DrawCommand::AddPolyLine(std::vector<glm::vec4> points, glm::vec4 color, float thickness, bool closed, glm::mat4 T, ImGuiID id)
	{
		auto screen_space = [](const glm::vec4 vertex) -> glm::vec3 {
			return glm::vec3(vertex.x / vertex.w, vertex.y / vertex.w, vertex.z / vertex.w);
		};

		auto screen_to_world = [](const glm::mat4& VPinv, const glm::vec2& q, float z) {
			glm::vec4 p = VPinv * glm::vec4(q, z, 1.0f);
			return p / p.w;
		};

		if (points.size() < 2)
			return;

		const int points_count = (int)points.size();
		size_t segments = points.size();

		if (!closed)
			segments = segments - 1;

		ImGui3DContext& g = *GImGui3D;
		const glm::vec2 screenSize(g.ScreenSize.x, g.ScreenSize.y);

		unsigned int currentIndex = data->size();

		const glm::mat4 A = g.ViewProjectionMatrix * T;

		// Temporary buffers
		std::vector<glm::vec3> screen_space_points;
		std::transform(points.begin(), points.end(), std::back_inserter(screen_space_points), [&A, &screen_space](const glm::vec4 p) -> glm::vec3 {
			return screen_space(A * p);
			});
		std::vector<glm::vec2> temp_normals(points_count);
		std::vector<glm::vec2> temp_points(2 * points_count);

		for (int i1 = 0; i1 < segments; ++i1) {
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			glm::vec2 dm = screen_space_points[i2] - screen_space_points[i1];
			IM3D_NORMALIZE2F_OVER_ZERO(dm.x, dm.y);
			temp_normals[i1] = glm::vec2(dm.y, -dm.x);

			if (i1 > 0 && glm::dot(temp_normals[i1], temp_normals[i1 - 1]) < 0) {
				temp_normals[i1] *= -1;
			}
		}
		if (!closed) {
			temp_normals[points_count - 1] = temp_normals[points_count - 2];
		}

		if (!closed)
		{
			temp_points[0] = glm::vec2(screen_space_points[0]) + temp_normals[0] / screenSize * thickness;
			temp_points[1] = glm::vec2(screen_space_points[0]) - temp_normals[0] / screenSize * thickness;
			temp_points[(points_count - 1) * 2 + 0] = glm::vec2(screen_space_points[points_count - 1]) + temp_normals[points_count - 1] / screenSize * thickness;
			temp_points[(points_count - 1) * 2 + 1] = glm::vec2(screen_space_points[points_count - 1]) - temp_normals[points_count - 1] / screenSize * thickness;
		}

		for (int i1 = 0; i1 < segments; ++i1) {
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;

			// Average normals
			glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
			IM3D_NORMALIZE2F_OVER_EPSILON_CLAMP(dm.x, dm.y, 0.000001f, 100.0f);

			// Create vertices
			temp_points[2 * i2 + 0] = glm::vec2(screen_space_points[i2]) - dm / screenSize * thickness;
			temp_points[2 * i2 + 1] = glm::vec2(screen_space_points[i2]) + dm / screenSize * thickness;
		}

		// create points
		std::vector<glm::vec4> line_vertices(temp_points.size());
		std::vector<glm::vec4> colors(temp_points.size(), color);
		
		for (int i = 0; i < (int)points.size(); ++i) { 
			line_vertices[2 * i + 0] = screen_to_world(g.ViewPerspectiveMatrixInverse, temp_points[2 * i + 0], screen_space_points[i].z);
			line_vertices[2 * i + 1] = screen_to_world(g.ViewPerspectiveMatrixInverse, temp_points[2 * i + 1], screen_space_points[i].z);
		}
		AddTriangleStrip(line_vertices, colors, glm::mat4(1), id);
		gl::IndexBuffer& indices = *batch.indexBuffer;

		if (closed) {
			indices.insert(indices.end(), { 
				(unsigned int)data->size() - 2, currentIndex, currentIndex + 1,
				(unsigned int)data->size() - 2, currentIndex + 1, (unsigned int)data->size() - 1
			});
		}
	}
	void DrawCommand::AddPolyLine(std::vector<glm::vec3> points, glm::vec4 color, float thickness, bool closed, glm::mat4 T, ImGuiID id)
	{
		auto screen_space = [](const glm::vec4 vertex) -> glm::vec3 {
			return glm::vec3(vertex.x / vertex.w, vertex.y / vertex.w, vertex.z / vertex.w);
		};

		auto screen_to_world = [](const glm::mat4& VPinv, const glm::vec2& q, float z) {
			glm::vec4 p = VPinv * glm::vec4(q, z, 1.0f);
			return p / p.w;
		};

		if (points.size() < 2)
			return;

		const int points_count = (int)points.size();
		size_t segments = points.size();

		if (!closed)
			segments = segments - 1;

		ImGui3DContext& g = *GImGui3D;
		const glm::vec2 screenSize(g.ScreenSize.x, g.ScreenSize.y);

		unsigned int currentIndex = data->size();

		const glm::mat4 A = g.ViewProjectionMatrix * T;

		// Temporary buffers
		std::vector<glm::vec3> screen_space_points;
		std::transform(points.begin(), points.end(), std::back_inserter(screen_space_points), [&A, &screen_space](const glm::vec3 p) -> glm::vec3 {
			return screen_space(A * glm::vec4(p, 1));
			});
		std::vector<glm::vec2> temp_normals(points_count);
		std::vector<glm::vec2> temp_points(2 * points_count);

		for (int i1 = 0; i1 < segments; ++i1) {
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			glm::vec2 dm = screen_space_points[i2] - screen_space_points[i1];
			IM3D_NORMALIZE2F_OVER_ZERO(dm.x, dm.y);
			temp_normals[i1] = glm::vec2(dm.y, -dm.x);

			if (i1 > 0 && glm::dot(temp_normals[i1], temp_normals[i1 - 1]) < 0) {
				temp_normals[i1] *= -1;
			}
		}
		if (!closed) {
			temp_normals[points_count - 1] = temp_normals[points_count - 2];
		}

		if (!closed)
		{
			temp_points[0] = glm::vec2(screen_space_points[0]) + temp_normals[0] / screenSize * thickness;
			temp_points[1] = glm::vec2(screen_space_points[0]) - temp_normals[0] / screenSize * thickness;
			temp_points[(points_count - 1) * 2 + 0] = glm::vec2(screen_space_points[points_count - 1]) + temp_normals[points_count - 1] / screenSize * thickness;
			temp_points[(points_count - 1) * 2 + 1] = glm::vec2(screen_space_points[points_count - 1]) - temp_normals[points_count - 1] / screenSize * thickness;
		}

		for (int i1 = 0; i1 < segments; ++i1) {
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;

			// Average normals
			glm::vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
			IM3D_NORMALIZE2F_OVER_EPSILON_CLAMP(dm.x, dm.y, 0.000001f, 100.0f);

			// Create vertices
			temp_points[2 * i2 + 0] = glm::vec2(screen_space_points[i2]) - dm / screenSize * thickness;
			temp_points[2 * i2 + 1] = glm::vec2(screen_space_points[i2]) + dm / screenSize * thickness;
		}

		// create points
		std::vector<glm::vec4> line_vertices(temp_points.size());
		std::vector<glm::vec4> colors(temp_points.size(), color);

		for (int i = 0; i < (int)points.size(); ++i) {
			line_vertices[2 * i + 0] = screen_to_world(g.ViewPerspectiveMatrixInverse, temp_points[2 * i + 0], screen_space_points[i].z);
			line_vertices[2 * i + 1] = screen_to_world(g.ViewPerspectiveMatrixInverse, temp_points[2 * i + 1], screen_space_points[i].z);
		}
		AddTriangleStrip(line_vertices, colors, glm::mat4(1), id);
		gl::IndexBuffer& indices = *batch.indexBuffer;

		if (closed) {
			indices.insert(indices.end(), {
				(unsigned int)data->size() - 2, currentIndex, currentIndex + 1,
				(unsigned int)data->size() - 2, currentIndex + 1, (unsigned int)data->size() - 1
				});
		}
	}

	void DrawCommand::AddLine(glm::vec4 from, glm::vec4 to, float width, glm::vec4 color, ImGuiID id)
	{
		ImGui3DContext& g = *GImGui3D;
		const glm::mat4& MVP = g.ViewProjectionMatrix * g.ModelMatrix;
		const glm::vec4 start = MVP * from;
		const glm::vec4 end = MVP * to;

		const glm::vec3 p(start.x / start.w, start.y / start.w, start.z / start.w);
		const glm::vec3 q(end.x / end.w, end.y / end.w, end.z / end.w);

		const glm::vec2 d = glm::normalize(glm::vec2(p) - glm::vec2(q));
		const glm::vec2 n = 0.5f * width * glm::vec2(d.y, -d.x) / glm::vec2(g.ScreenSize.x, g.ScreenSize.y);

		const glm::vec2 nAlias = (0.5f * width + 2.0f) * glm::vec2(d.y, -d.x) / glm::vec2(g.ScreenSize.x, g.ScreenSize.y);
		const glm::vec4 aliasColor(color.r, color.g, color.b, 0.f);


		glm::vec4 p0(glm::vec2(p) + n, p.z, 1.f);
		glm::vec4 p1(glm::vec2(p) - n, p.z, 1.f);
		glm::vec4 p2(glm::vec2(q) + n, q.z, 1.f);
		glm::vec4 p3(glm::vec2(q) - n, q.z, 1.f);

		glm::vec4 q0(glm::vec2(p) + nAlias, p.z, 1.f);
		glm::vec4 q1(glm::vec2(p) - nAlias, p.z, 1.f);
		glm::vec4 q2(glm::vec2(q) + nAlias, q.z, 1.f);
		glm::vec4 q3(glm::vec2(q) - nAlias, q.z, 1.f);

		AddTriangleStrip(
			std::vector<glm::vec4>{ q2, q0, p2, p0, p3, p1, q3, q1 }, 
			{ aliasColor, aliasColor, color, color, color, color, aliasColor, aliasColor },
			g.ViewPerspectiveMatrixInverse, id);
	}
	void DrawCommand::AddLine(glm::vec3 from, glm::vec3 to, float width, glm::vec4 color, ImGuiID id)
	{
		AddLine(glm::vec4(from, 1), glm::vec4(to, 1), width, color, id);
	}

	void DrawCommand::AddFilledSemiCircle(glm::vec4 pos, glm::vec4 axis, float radius, float startAngle, float endAngle, glm::vec4 color, int segments, ImGuiID id)
	{
		AddFilledSemiCircle(glm::vec3(pos), glm::vec3(axis), radius, startAngle, endAngle, color, segments, id);
	}
	void DrawCommand::AddFilledSemiCircle(glm::vec3 pos, glm::vec3 axis, float radius, float startAngle, float endAngle, glm::vec4 color, int segments, ImGuiID id)
	{
		const glm::mat4 ModelMatrix = glm::translate(pos) * glm::transpose(zAxistoWorld(glm::vec4(axis, 1)));
		
		float deltaAngle = (endAngle - startAngle) / (segments - 1);

		const unsigned int centerId = data->size();
		data->push_back(glm::vec4(pos, 1), color, id, glm::vec2(0));
		gl::IndexBuffer& indices = *batch.indexBuffer;

		for (unsigned int i = 0; i < segments; ++i) {
			const float angle = startAngle + i * deltaAngle;
			const glm::vec3 p = radius * glm::vec3(std::cos(angle), std::sin(angle), 0.0f);
			data->push_back(ModelMatrix * glm::vec4(p, 1.0f), color, id, glm::vec2(0));
			if (i > 0) {
				indices.insert(indices.end(), { centerId, centerId + i - 1, centerId + i });
			}
		}
	}

	void DrawCommand::AddFilledSemiCircle(glm::vec4 pos, glm::vec4 startDirection, glm::vec4 axis, float angle, glm::vec4 color, int segments, ImGuiID id)
	{
		AddFilledSemiCircle(glm::vec3(pos), glm::vec3(startDirection), glm::vec3(axis), angle, color, segments, id);
	}
	void DrawCommand::AddFilledSemiCircle(glm::vec3 pos, glm::vec3 startDirection, glm::vec3 axis, float angle, glm::vec4 color, int segments, ImGuiID id)
	{
		const float radius = std::sqrt(glm::dot(startDirection, startDirection));
		const glm::vec3 u = glm::normalize(startDirection);

		const unsigned int centerId = data->size();
		data->push_back(glm::vec4(pos, 1), color, id, glm::vec2(0));

		const float delta = angle / (segments - 1);
		gl::IndexBuffer& indices = *batch.indexBuffer;

		for (unsigned int i = 0; i < segments; ++i) {
			const float t = i * delta;
			data->push_back(glm::vec4(pos + radius * glm::rotate(u, t, axis), 1.0f), color, id, glm::vec2(0));
			if (i > 0) {
				indices.insert(indices.end(), { centerId, centerId + i - 1, centerId + i });
			}
		}
	}

	void DrawCommand::AddFilledAxisAlignedCube(glm::vec4 pos, glm::vec3 size, glm::vec4 color, ImGuiID id)
	{
		size *= .5f;
		const glm::vec4 p000(pos.x - size.x, pos.y - size.y, pos.z - size.z, 1);
		const glm::vec4 p001(pos.x - size.x, pos.y - size.y, pos.z + size.z, 1);
		const glm::vec4 p101(pos.x + size.x, pos.y - size.y, pos.z + size.z, 1);
		const glm::vec4 p100(pos.x + size.x, pos.y - size.y, pos.z - size.z, 1);

		const glm::vec4 p010(pos.x - size.x, pos.y + size.y, pos.z - size.z, 1);
		const glm::vec4 p011(pos.x - size.x, pos.y + size.y, pos.z + size.z, 1);
		const glm::vec4 p111(pos.x + size.x, pos.y + size.y, pos.z + size.z, 1);
		const glm::vec4 p110(pos.x + size.x, pos.y + size.y, pos.z - size.z, 1);

		const unsigned int i0 = data->size();

		data->push_back(p000, color, id, glm::vec2(0));
		data->push_back(p001, color, id, glm::vec2(0));
		data->push_back(p101, color, id, glm::vec2(0));
		data->push_back(p100, color, id, glm::vec2(0));

		data->push_back(p010, color, id, glm::vec2(0));
		data->push_back(p011, color, id, glm::vec2(0));
		data->push_back(p111, color, id, glm::vec2(0));
		data->push_back(p110, color, id, glm::vec2(0));

		gl::IndexBuffer& indices = *batch.indexBuffer;
		indices.insert(indices.end(), {
			i0, i0 + 1, i0 + 2,
			i0, i0 + 2, i0 + 3,

			i0 + 4, i0 + 5, i0 + 6,
			i0 + 4, i0 + 6, i0 + 7,

			i0 + 4, i0, i0 + 1,
			i0 + 4, i0 + 1, i0 + 5,

			i0 + 7, i0 + 3, i0 + 2,
			i0 + 7, i0 + 2, i0 + 6
			});
	}

	void DrawCommand::AddFilledAxisAlignedCube(glm::vec3 pos, glm::vec3 size, glm::vec4 color, ImGuiID id)
	{
		AddFilledAxisAlignedCube(glm::vec4(pos, 1), size, color, id);
	}

	void DrawCommand::AddAxisAlignedCube(glm::vec4 pos, glm::vec3 size, float thickness, glm::vec4 color, ImGuiID id)
	{
		if (thickness < 0) {
			AddFilledAxisAlignedCube(pos, size, color, id);
		}

		size *= .5f;
		const glm::vec4 p000(pos.x - size.x, pos.y - size.y, pos.z - size.z, 1);
		const glm::vec4 p001(pos.x - size.x, pos.y - size.y, pos.z + size.z, 1);
		const glm::vec4 p101(pos.x + size.x, pos.y - size.y, pos.z + size.z, 1);
		const glm::vec4 p100(pos.x + size.x, pos.y - size.y, pos.z - size.z, 1);

		const glm::vec4 p010(pos.x - size.x, pos.y + size.y, pos.z - size.z, 1);
		const glm::vec4 p011(pos.x - size.x, pos.y + size.y, pos.z + size.z, 1);
		const glm::vec4 p111(pos.x + size.x, pos.y + size.y, pos.z + size.z, 1);
		const glm::vec4 p110(pos.x + size.x, pos.y + size.y, pos.z - size.z, 1);

		AddLine(p000, p001, thickness, color, id);
		AddLine(p001, p101, thickness, color, id);
		AddLine(p101, p100, thickness, color, id);
		AddLine(p100, p000, thickness, color, id);

		AddLine(p010, p011, thickness, color, id);
		AddLine(p011, p111, thickness, color, id);
		AddLine(p111, p110, thickness, color, id);
		AddLine(p110, p010, thickness, color, id);


		AddLine(p000, p010, thickness, color, id);
		AddLine(p100, p110, thickness, color, id);
		AddLine(p101, p111, thickness, color, id);
		AddLine(p001, p011, thickness, color, id);
	}

	void DrawCommand::AddAxisAlignedCube(glm::vec3 pos, glm::vec3 size, float thickness, glm::vec4 color, ImGuiID id)
	{
		AddAxisAlignedCube(glm::vec4(pos, 1), size, thickness, color, id);
	}

	void DrawCommand::AddIndexedFaceSet(const std::vector<glm::vec4>& vertices, const std::vector<unsigned int>& indices, glm::vec4 color, ImGuiID id)
	{
		const unsigned int i0 = data->size();
		for (size_t i = 0; i < vertices.size(); ++i) {
			data->push_back(vertices[i], color, id, glm::vec2(0));
		}
		std::transform(indices.begin(), indices.end(), std::back_inserter(*batch.indexBuffer), [i0](unsigned int i) {
			return i0 + i;
		});
	}
	
	void DrawCommand::AddCircle(glm::vec4 pos, glm::vec4 axis, float radius, float width, glm::vec4 color, int segments, ImGuiID id)
	{
		AddCircle(glm::vec3(pos), glm::vec3(axis), radius, width, color, segments, id);
	}
	void DrawCommand::AddCircle(glm::vec3 pos, glm::vec3 axis, float radius, float width, glm::vec4 color, int segments, ImGuiID id)
	{
		const glm::mat4 ModelMatrix = glm::translate(pos) * glm::transpose(zAxistoWorld(glm::vec4(axis, 1)));

		float deltaAngle = glm::two_pi<float>() / segments;

		std::vector<glm::vec4> points;

		for (unsigned int i = 0; i < segments; ++i) {
			const float angle = i * deltaAngle;
			const glm::vec3 p = radius * glm::vec3(std::cos(angle), std::sin(angle), 0.0f);
			points.push_back(glm::vec4(p, 1));
		}
		AddPolyLine(points, color, width, true, ModelMatrix, id);
	}

	void DrawCommand::AddFilledArc(glm::vec4 pos, glm::vec4 from, glm::vec4 to, float radius, glm::vec4 color, int segments, ImGuiID id)
	{
		AddFilledArc(glm::vec3(pos), glm::vec3(from), glm::vec3(to), radius, color, segments, id);
	}
	void DrawCommand::AddFilledArc(glm::vec3 pos, glm::vec3 from, glm::vec3 to, float radius, glm::vec4 color, int segments, ImGuiID id)
	{
		const glm::vec3 A = from - pos;
		const glm::vec3 B = to - pos;

		const glm::vec3 u = glm::normalize(A);
		const glm::vec3 v = glm::normalize(B);

		const float delta = 1.0f / (segments - 1);
		const float dot = glm::dot(u, v);
		float angle = std::acos(dot);
		if (angle < 0) {
			std::cout << "Negative angle" << std::endl;
		}

		float invSinAngle = std::sin(angle);
		invSinAngle = (std::abs(invSinAngle) < 1e-8f) ? 1.0f / 1e-8f : 1.0f / invSinAngle;

		const unsigned int centerId = data->size();
		data->push_back(glm::vec4(pos, 1), color, id, glm::vec2(0));
		gl::IndexBuffer& indices = *batch.indexBuffer;

		for (unsigned int i = 0; i < segments; ++i) {
			const float t = i * delta;
			// slerp
			glm::vec3 p = std::sin((1.0f - t) * angle) * invSinAngle * A + std::sin(t * angle) * invSinAngle * B;
			data->push_back(glm::vec4(pos + radius * p, 1.0f), color, id, glm::vec2(0));
			if (i > 0) {
				indices.insert(indices.end(), { centerId, centerId + i - 1, centerId + i });
			}
		}
	}

	void DrawCommand::AddFilledScreenSpaceCircle(ImVec2 pos, float radius, glm::vec4 color, unsigned int segments, float depth, ImGuiID id)
	{
		// To clip space
		ImGui3DContext& g = *GImGui3D;
		ImVec2 q = toClipSpace(pos);
		
		unsigned int cId = data->size();
		data->push_back(g.ViewPerspectiveMatrixInverse * glm::vec4(q.x, q.y, depth, 1.f), color, id, glm::vec2(0));
		const float aliasRadius = 2.0f;
		glm::vec4 aliasColor(color.r, color.g, color.b, 0.0f);

		gl::IndexBuffer& indices = *batch.indexBuffer;
		const float delta = glm::two_pi<float>() / segments;

		radius *= 2;

		const unsigned int i0 = cId + 1;
		for (int i = 0; i < (int)segments; ++i) {
			const float angle = i * delta;
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			const glm::vec4 p0(q.x + radius * c / g.ScreenSize.x, q.y + radius * s / g.ScreenSize.y, depth, 1.f);
			const glm::vec4 p1(q.x + (radius + aliasRadius) * c / g.ScreenSize.x, q.y + (radius + aliasRadius) * s / g.ScreenSize.y, depth, 1.f);
			data->push_back(g.ViewPerspectiveMatrixInverse * p0, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p1, aliasColor, id, glm::vec2(0));

			const unsigned int idx0 = i0 + 2 * i;
			const unsigned int idx1 = i0 + 2 * ((i + 1) % segments);
			indices.insert(indices.end(), {
				cId, idx0, idx1,
				idx0, idx0 + 1, idx1,
				idx0 + 1, idx1, idx1 + 1
				});
		}

	}

	void DrawCommand::AddScreenSpaceCircle(ImVec2 pos, float radius, float width, glm::vec4 color, unsigned int segments, float depth, ImGuiID id)
	{
		if (width < 0) {
			AddFilledScreenSpaceCircle(pos, radius, color, segments, id);
			return;
		}

		ImGui3DContext& g = *GImGui3D;

		ImVec2 q = toClipSpace(pos);
		const float aliasRadius = 2.0f;
		glm::vec4 aliasColor(color.r, color.g, color.b, 0.0f);

		gl::IndexBuffer& indices = *batch.indexBuffer;
		const float delta = glm::two_pi<float>() / segments;

		const float router = radius + 0.5f * width;
		const float rinner = radius - 0.5f * width;

		const unsigned int i0 = data->size();
		for (int i = 0; i < (int)segments; ++i) {
			const float angle = i * delta;
			const float c = std::cos(angle);
			const float s = std::sin(angle);
			const glm::vec4 p0(q.x + router * c / g.ScreenSize.x, q.y + router * s / g.ScreenSize.y, depth, 1.f);
			const glm::vec4 p1(q.x + (router + aliasRadius) * c / g.ScreenSize.x, q.y + (router + aliasRadius) * s / g.ScreenSize.y, depth, 1.f);
			const glm::vec4 p2(q.x + rinner * c / g.ScreenSize.x, q.y + rinner * s / g.ScreenSize.y, depth, 1.f);
			const glm::vec4 p3(q.x + (rinner - aliasRadius) * c / g.ScreenSize.x, q.y + (rinner - aliasRadius) * s / g.ScreenSize.y, depth, 1.f);
			data->push_back(g.ViewPerspectiveMatrixInverse * p0, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p1, aliasColor, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p2, color, id, glm::vec2(0));
			data->push_back(g.ViewPerspectiveMatrixInverse * p3, aliasColor, id, glm::vec2(0));

			const unsigned int idx0 = i0 + 4 * i;
			const unsigned int idx1 = i0 + 4 * ((i + 1) % segments);
			indices.insert(indices.end(), {
				idx0, idx0 + 1, idx1,
				idx0 + 1, idx1, idx1 + 1,
				idx0, idx0 + 2, idx1 + 2,
				idx0, idx1 + 2, idx1,
				idx0 + 2, idx0 + 3, idx1 + 3,
				idx0 + 2, idx1 + 3, idx1 + 2
				});
		}
	}

	void DrawCommand::AddScreenSpaceLine(ImVec2 start, ImVec2 end, float width, glm::vec4 color, float depth, ImGuiID id)
	{
		ImGui3DContext& g = *GImGui3D;

		const ImVec2 p = toClipSpace(start);
		const ImVec2 q = toClipSpace(end);

		const glm::vec2 d = glm::normalize(glm::vec2(p.x, p.y) - glm::vec2(q.x, q.y));
		const glm::vec2 n = 0.5f * width * glm::vec2(d.y, -d.x) / glm::vec2(g.ScreenSize.x, g.ScreenSize.y);

		const glm::vec2 nAlias = (0.5f * width + 2.0f) * glm::vec2(d.y, -d.x) / glm::vec2(g.ScreenSize.x, g.ScreenSize.y);
		const glm::vec4 aliasColor(color.r, color.g, color.b, 0.f);


		glm::vec4 p0(glm::vec2(p.x, p.y) + n, depth, 1.f);
		glm::vec4 p1(glm::vec2(p.x, p.y) - n, depth, 1.f);
		glm::vec4 p2(glm::vec2(q.x, q.y) + n, depth, 1.f);
		glm::vec4 p3(glm::vec2(q.x, q.y) - n, depth, 1.f);

		glm::vec4 q0(glm::vec2(p.x, p.y) + nAlias, depth, 1.f);
		glm::vec4 q1(glm::vec2(p.x, p.y) - nAlias, depth, 1.f);
		glm::vec4 q2(glm::vec2(q.x, q.y) + nAlias, depth, 1.f);
		glm::vec4 q3(glm::vec2(q.x, q.y) - nAlias, depth, 1.f);

		AddTriangleStrip(
			std::vector<glm::vec4>{ q2, q0, p2, p0, p3, p1, q3, q1 },
			{ aliasColor, aliasColor, color, color, color, color, aliasColor, aliasColor },
			g.ViewPerspectiveMatrixInverse, id);
	}
}