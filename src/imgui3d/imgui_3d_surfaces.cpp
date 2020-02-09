#include "imgui_3d_surfaces.h"

#include "imgui3d/imgui_3d.h"

void ImGui3D::ParametricSurface(std::function<glm::vec3(float, float)> S, glm::vec4 color, ImVec4 domain, ImVec2 gridSize, unsigned int subdivions)
{
	const unsigned int cols = static_cast<unsigned int>(gridSize.x)* subdivions + 1;
	const unsigned int rows = static_cast<unsigned int>(gridSize.y)* subdivions + 1;

	std::vector<glm::vec4> points(rows * cols);
	std::vector<unsigned int> indices;
	
	ImGui3D::ImGui3DContext& g = *ImGui3D::GImGui3D;
	ImGui3D::DrawCommand& drawList = *g.currentDrawList();
	const ImGui3D::ImGui3DStyle& style = g.Style;

	const float du = (domain.z - domain.x) / (cols - 1);
	const float dv = (domain.w - domain.y) / (rows - 1);

	for (unsigned int j = 0; j < rows; ++j) {
		for (unsigned int i = 0; i < cols; ++i) {
			points[j * rows + i] = glm::vec4(S(du * i, dv * j), 1);

			if (i > 0 && j > 0) {
				indices.insert(indices.end(), {
					(j - 1) * rows + i - 1, j * rows + i - 1, j * rows + i,
					(j - 1) * rows + i - 1, j * rows + i, (j - 1) * rows + i
				});
			}
			if (j > 0 && i % subdivions == 0) {
				drawList.AddLine(points[j * rows + i], points[(j - 1) * rows + i], style.GridLineWidth, style.Colors[ImGui3DCol_Grid]);
			}
			if (i > 0 && j % subdivions == 0) {
				drawList.AddLine(points[j * rows + i], points[j * rows + i - 1], style.GridLineWidth, style.Colors[ImGui3DCol_Grid]);
			}
		}
	}

	drawList.AddIndexedFaceSet(points, indices, color);
}
