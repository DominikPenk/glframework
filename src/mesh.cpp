#include "mesh.hpp"
#include "renderer.hpp"

#include <numeric>
#include <eigen3/Eigen/Geometry>

#include <glm/gtx/matrix_cross_product.hpp>

#include "imgui3d/imgui_3d.h"

using namespace gl;
using namespace Eigen;

gl::TriangleMesh::TriangleMesh() :
	Mesh()
{
	mColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	mBatch.shader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	mVertices = mBatch.addVertexAttribute<float, 3>(0);
}

gl::TriangleMesh::TriangleMesh(const std::vector<Eigen::Vector3f>& vertices, std::vector<Eigen::Vector3i>& indices) :
	gl::TriangleMesh()
{
	for (const Eigen::Vector3f& p : vertices) {
		mVertices->push_back(p);
	}
	IndexBuffer& indexBuffer = *mBatch.indexBuffer;
	indexBuffer.resize(indices.size() * 3);
	for (size_t i = 0; i < indices.size(); ++i) {
		indexBuffer(3 * i) = indices[i].x();
		indexBuffer(3 * i + 1) = indices[i].y();
		indexBuffer(3 * i + 2) = indices[i].z();
	}
}

void gl::TriangleMesh::render(const gl::Renderer* env)
{

	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;

	mBatch.execute("MVP", MVP, "color", mColor);
}

void gl::TriangleMesh::addTriangles(std::vector<Eigen::Vector3f>& vertices)
{
	assert(vertices.size() % 3 == 0);
	unsigned int i0 = mVertices->size();
	for(int i = 0; i < vertices.size(); ++i) {
		mVertices->push_back(Eigen::Vector3f(vertices[i].x(), vertices[i].y(), vertices[i].z()));
		mBatch.indexBuffer->push_back(i0 + i);
	}
}

void gl::TriangleMesh::removeDoubles(float thr)
{
	std::vector<unsigned int> mapping(mVertices->size());
	std::iota(mapping.begin(), mapping.end(), 0);
	for (size_t i = 0; i < mVertices->size(); ++i) {
		if (mapping[i] != i) continue;
#pragma omp parallel for
		for (int j = i + 1; j < (int)mVertices->size(); ++j) {
			if (mapping[j] != j) continue;
			if ((mVertices->at(i) - mVertices->at(j)).squaredNorm() < thr) {
				mapping[j] = (unsigned int)i;
			}
		}
	}

	// Update vertices and indices with mapping
	std::vector<decltype(mVertices)::element_type::value_type> newVertices;
	std::vector<unsigned int> indexMap;
	for (size_t i = 0; i < mapping.size(); ++i) {
		if (mapping[i] == i) {
			indexMap.push_back(newVertices.size());
			newVertices.push_back((*mVertices)[i]);
		}
		else {
			indexMap.push_back(indexMap[mapping[i]]);
		}
	}
	mVertices->resize(newVertices.size());
	std::copy(newVertices.begin(), newVertices.end(), mVertices->begin());

	IndexBuffer& indexBuffer = *mBatch.indexBuffer;

	#pragma omp parallel for
	for (int i = 0; i < (int)indexBuffer.size(); ++i) {
		indexBuffer[i] = indexMap[mapping[indexBuffer[i]]];
	}
}

bool gl::TriangleMesh::handleIO(const Renderer* env, ImGuiIO& io)
{
	bool updated = false;
	for (auto& vertex : *mVertices) {
		updated |= ImGui3D::Vertex(vertex.data());
	}
	mVertices->setDirty(updated);
	return updated;
}

gl::Mesh::Mesh() :
	mShowInOutliner(true),
	visible(true),
	name("Mesh"),
	ModelMatrix(1)
{
}

void gl::Mesh::drawOutliner()
{
}


gl::CoordinateFrame::CoordinateFrame(float length) :
	Mesh(),
	axisLength(length)
{
	mPoints.push_back(Eigen::Vector3f(0, 0, 0));
	mPoints.target() = GL_ARRAY_BUFFER;
	mPoints.usage() = GL_DYNAMIC_DRAW;

	mVAO.addVertexAttribute(mPoints, 0);

	mShader = std::string(GL_FRAMEWORK_SHADER_DIR) + "axis.glsl";

	mShowInOutliner = false;
}

void gl::CoordinateFrame::render(const Renderer* env)
{
	mPoints.update();

	auto _ = mShader.use();

	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;
	mShader.setUniform("MVP", MVP);
	mShader.setUniform("length", axisLength);

	mVAO.bind();
	glDrawArrays(GL_POINTS, 0, 1);
	mVAO.unbind();

	glUseProgram(0);
}

glm::mat4 rotationFromUnitVectors(const glm::vec3& a, const glm::vec3& b) {
	glm::vec3 v = glm::cross(a, b);
	glm::mat4 V = glm::matrixCross4(v);
	float c = glm::dot(a, b);
	return glm::mat4(1.0f) + V + 1.0f / (1.0f + c) * V * V;
}

gl::Plane::Plane(glm::vec3 position, glm::vec3 normal, glm::vec2 dimensions) :
	Mesh(),
	dimensions(dimensions),
	color(0.25, 0.25, 0.25, 1.0)
{
	mShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "plane.glsl");
	ModelMatrix = rotationFromUnitVectors(glm::vec3(0, 1, 0), glm::normalize(normal));
	ModelMatrix[3] = glm::vec4(position, 1);
}


void gl::Plane::render(const Renderer* env)
{

	auto _ = mShader.use();
	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4  VP = P * V * ModelMatrix;
	mShader.setUniform("MVP", VP);
	mShader.setUniform("dimensions", dimensions);
	mShader.setUniform("color", color);
	
	mVAO.bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	mVAO.unbind();
	glUseProgram(0);
}
