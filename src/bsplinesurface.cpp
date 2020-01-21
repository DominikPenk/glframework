#include "bsplinesurface.hpp"

#include <eigen3/Eigen/Core>

#include "gl/IconsFontAwesome5.h"
#include "imgui.h"

#include "imgui/imgui_utils.h"
#include "imgui/imgui_orient.h"

#include <gl/imgui3d/imgui_3d.h>

#include "Heightmap.hpp"
#include "renderer.hpp"


using namespace Eigen;

gl::BSplineSurfaceMesh::BSplineSurfaceMesh() :
	Mesh(),
	mShowControlMesh(false),
	mDisplayStyle(DisplayStyle::FLAT),
	mRegion(Region::NoCheck()),
	plane(1, 0, 0, 0)
{
	color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	mBatchSpline.primitiveType = GL_PATCHES;
	mBatchSpline.patchsize = 16;
	mBatchSpline.shader = Shader(std::string(SHADER_DIR) + "bsplinesurface.glsl");
	mPoints = mBatchSpline.addVertexAttribute<float, 3>(0);
	mUVs = mBatchSpline.addVertexAttribute<float, 2>(1);

	// Set reasonable values for mRegion
	mRegion.cx = mRegion.cy = mRegion.alpha = 0;
	mRegion.a = 10;
	mRegion.b = 10;

	// Set up control mesh
	colorControlMesh = glm::vec4(.1f, .1f, .1f, 1.0f);
	mBatchControlMesh.addVertexAttribute(0, mPoints);
	mBatchControlMesh.shader = Shader(std::string(SHADER_DIR) + "shadeless.glsl");
}

gl::BSplineSurfaceMesh::BSplineSurfaceMesh(const std::vector<Eigen::Vector3f>& vertices) :
	gl::BSplineSurfaceMesh()
{
	size_t n = std::floor(std::sqrt(vertices.size()));
	if (n * n != vertices.size())
		throw new std::invalid_argument("Invalid number of vertices");

	float delta = 1.0f / (n - 1);

	mPoints->insert(mPoints->end(), vertices.begin(), vertices.end());

	for (size_t j = 0; j < n; ++j) {
		for (size_t i = 0; i < n; ++i) {
			mUVs->push_back(Eigen::Vector2f((float)i * delta, (float)j * delta));
			if (i > 0 && j > 0) {
				mBatchSpline.indexBuffer->push_back((j - 1) * n + (i - 1));
				mBatchSpline.indexBuffer->push_back((j - 1) * n + i);
				mBatchSpline.indexBuffer->push_back(j * n + (i - 1));
				mBatchSpline.indexBuffer->push_back(j * n + i);
			}
		}
	}

	constructControlPolygon();
}

gl::BSplineSurfaceMesh::BSplineSurfaceMesh(const Eigen::Vector2f& topLeft, float spacing, int n) :
	gl::BSplineSurfaceMesh()
{
	float delta = 1.0f / (n - 1);

	Eigen::Vector3f tl(topLeft.x(), topLeft.y(), 0.0f);

	for (int j = 0; j < n; ++j) {
		for (int i = 0; i < n; ++i) {
			mPoints->push_back(tl + spacing * Eigen::Vector3f(i, j, 0.f));	// d_{j, i}
			mUVs->push_back(Eigen::Vector2f(i * delta, j * delta));			// This is actually (t_{i + 3}, t_{j + 3})
			if (i >= 3 && j >= 3) {
				for (int dj = -3; dj < 1; ++dj) {
					for (int di = -3; di < 1; ++di) {
						mBatchSpline.indexBuffer->push_back((j + dj) * n + i + di);
					}
				}
			}
		}
	}

	constructControlPolygon();
}

gl::BSplineSurfaceMesh::BSplineSurfaceMesh(const Heightmap& map) :
	gl::BSplineSurfaceMesh()
{
	std::vector<Eigen::Vector4f> cps = map.controllPoints();
	size_t n = map.rows();

	float tmin = map.minT();
	float delta = map.spacing();
	for (int j = 0; j < map.rows(); ++j) {
		for (int i = 0; i < map.cols(); ++i) {
			mPoints->push_back(map.controllPoint(i, j).head<3>());
			mUVs->push_back(Eigen::Vector2f(tmin + delta * i, tmin + delta * j));
			if (i >= 3 && j >= 3) {
				for (int dj = -3; dj < 1; ++dj) {
					for (int di = -3; di < 1; ++di) {
						mBatchSpline.indexBuffer->push_back((j + dj) * n + i + di);
					}
				}
			}
		}
	}

	constructControlPolygon();
}

gl::BSplineSurfaceMesh::BSplineSurfaceMesh(const std::string path) :
	gl::BSplineSurfaceMesh(Heightmap(path))
{
}

void gl::BSplineSurfaceMesh::render(const Renderer* env)
{
	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;

	// Draw control Mesh
	if (mShowControlMesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mBatchControlMesh.execute("MVP", MVP, "color", colorControlMesh);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	mBatchSpline.execute(
		"MVP", MVP,
		"color", color,
		"displayStyle", static_cast<int>(mDisplayStyle),
		"regionType", static_cast<int>(mRegion.type),
		"ab", glm::vec2(mRegion.a, mRegion.b),
		"center", glm::vec2(mRegion.cx, mRegion.cy),
		"alpha", glm::radians(mRegion.alpha));
}

void gl::BSplineSurfaceMesh::drawOutliner()
{
	static const char* displayStyles[] = { "Flat", "Position", "UV", "Diopter (Linear)", "Diopter (HSV)" };
	if (ImGui::Button(mShowControlMesh ? ICON_FA_BORDER_ALL : ICON_FA_BORDER_NONE)) {
		mShowControlMesh = !mShowControlMesh;
	}
	ImGui::SameLine();
	if (ImGui::BeginCombo("Display Style", displayStyles[static_cast<int>(mDisplayStyle)])) {
		for (int i = 0; i < 5; ++i) {
			bool isSelected = i == static_cast<int>(mDisplayStyle);
			if (ImGui::Selectable(displayStyles[i], isSelected)) {
				mDisplayStyle = static_cast<DisplayStyle>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	if (mDisplayStyle == DisplayStyle::DISTANCE) {
		ImGui::InputDirection("Normal", (float*)&plane);
		ImGui::InputFloat("Distance", &plane.w);
	}
	if (ImGui::TreeNode("Color")) {
		ImGui::ColorPicker3("##Color", (float*)&color);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Grid Color")) {
		ImGui::ColorPicker4("##ColorGrid", (float*)&colorControlMesh);
		ImGui::TreePop();
	}
	ImGui::InputRegion("Region", &mRegion);
}

bool gl::BSplineSurfaceMesh::handleIO(const Renderer* env, ImGuiIO& io)
{
	static const float axis[] = { 0, 0, 1 };
	bool updated = false;
	if (mShowControlMesh) {
		for (auto& vertex : *mPoints) {
			updated |= ImGui3D::RestrictedVertex(vertex.data(), axis, ModelMatrix);
		}
	}
	mPoints->setDirty(updated);

	updated |= ImGui3D::TransformGizmo(ModelMatrix);
	return updated;
}

void gl::BSplineSurfaceMesh::constructControlPolygon()
{
	int n = (int)std::sqrt(mPoints->size());
	for (int j = 1; j < n; ++j) {
		for (int i = 1; i < n; ++i) {
			mBatchControlMesh.indexBuffer->push_back((j - 1) * n + i - 1);
			mBatchControlMesh.indexBuffer->push_back(j * n + i - 1);
			mBatchControlMesh.indexBuffer->push_back(j * n + i);

			mBatchControlMesh.indexBuffer->push_back((j - 1) * n + i - 1);
			mBatchControlMesh.indexBuffer->push_back(j * n + i);
			mBatchControlMesh.indexBuffer->push_back((j - 1) * n + i);
		}
	}
}