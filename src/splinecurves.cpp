#include "glpp/splinecurves.hpp"

#include <glm/gtx/intersect.hpp>

#include "glpp/imgui.hpp"

#include "glpp/renderer.hpp"

#include "glpp/imgui3d/imgui_3d.h"

void computeCatmulRomData(
	const std::vector<glm::vec3>& points,
	std::shared_ptr<gl::VertexBufferObject<float, 3>>& p,
	gl::VertexBufferObject<unsigned int, 1>& indices) {

	for (int i = 0; i < (int)points.size(); ++i) {
		const glm::vec3 point = points[i];
		indices.insert(indices.end(), {
			(uint)(i == 0 ? points.size() - 1 : i - 1),
			(uint)(i),
			(uint)((i + 1) % points.size()),
			(uint)((i + 2) % points.size())
 		});
		p->push_back(Eigen::Vector3f(point.x, point.y, point.z));
	}
}

gl::CatmullRomSpline::CatmullRomSpline(const std::vector<glm::vec3>& points) :
	Mesh(),
	color(1.0f, 0.0f, 1.0f, 1.0f),
	subdivisions(16),
	endpointCondition(EndpointCondition::Natural),
	q0(glm::vec3(0, 0, 0)),
	qk(glm::vec3(0, 0, 0)),
	linewidth(2),
	mPoints(std::make_shared<decltype(mPoints)::element_type>())
{
	mIndices.target() = GL_ELEMENT_ARRAY_BUFFER;
	mIndices.usage() = GL_DYNAMIC_DRAW;

	mVAO.setIndexBufferObject(mIndices);
	mVAO.addVertexAttribute(mPoints, 0);

	mShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "catmullromspline.glsl");

	computeCatmulRomData(points, mPoints, mIndices);
}

void gl::CatmullRomSpline::render(const gl::RendererBase * env)
{

	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4  VP = P * V;

	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto _ = mShader.use();
		
		mShader.setUniform("subdivisions", subdivisions);
		mShader.setUniform("MV", V * ModelMatrix);
		mShader.setUniform("P", P);
		mShader.setUniform("color", color);
		mShader.setUniform("nPoints", (int)mPoints->size() - 4);
		mShader.setUniform("endpointCondition", static_cast<int>(endpointCondition));
		mShader.setUniform("q0", q0);
		mShader.setUniform("qk", qk);
		mShader.setUniform("width", (float)linewidth / (env->camera()->ScreenWidth));

		mVAO.bind();
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		int n = endpointCondition == EndpointCondition::Periodic
			? mIndices.size()
			: mIndices.size() - 4;
		glDrawElements(GL_PATCHES, n, GL_UNSIGNED_INT, 0);
		mVAO.unbind();

		glDisable(GL_BLEND);
	}
	
}

void gl::CatmullRomSpline::drawOutliner()
{
	static const char* endPointConditions[] = {
		"Natural", "Periodic", "Clamped", "knot-a-knot"
	};
	ImGui::DragInt("Linewidth", &linewidth, 1.0f, 1, 100);

	ImGui::DragInt("Subdivisions", &subdivisions, 1.0f, 1, 64);
	if (ImGui::TreeNode("Color")) {
		ImGui::ColorPicker4("Curve Color", (float*)&color);
		ImGui::TreePop();
	}
	if (ImGui::BeginCombo("Display Style", endPointConditions[static_cast<int>(endpointCondition)])) {
		for (int i = 0; i < 4; ++i) {
			bool isSelected = i == static_cast<int>(endpointCondition);
			if (ImGui::Selectable(endPointConditions[i], isSelected)) {
				endpointCondition = static_cast<EndpointCondition>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	if (endpointCondition == EndpointCondition::Clamped) {
		ImGui::InputFloat3("q0", (float*)&q0);
		ImGui::InputFloat3("qk", (float*)&qk);
	}
}

bool gl::CatmullRomSpline::handleIO(const Renderer* env, ImGuiIO& io)
{
	bool wasChanged = false;
	for (unsigned int i = 0; i < mPoints->size(); ++i) {
		wasChanged |= ImGui3D::Vertex(&(mPoints->at(i).x()));
	}
	mPoints->setDirty(wasChanged);
	return wasChanged;
}
