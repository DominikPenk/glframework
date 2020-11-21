#include "glpp/mesh.hpp"
#include "glpp/renderer.hpp"

#include <numeric>
#include <eigen3/Eigen/Geometry>

#include <glm/gtx/matrix_cross_product.hpp>

#include "glpp/imgui3d/imgui_3d.h"

#ifdef WITH_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

using namespace gl;
using namespace Eigen;

gl::TriangleMesh::TriangleMesh() :
	Mesh(),
	visualizeNormals(false)
{
	mColor = glm::vec4(0.7f, 0.8f, 0.7f, 1.0f);

	mShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	mNormalShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle_normal.glsl");

	mVertexData = mBatch.addVertexAttributes<glm::vec3, glm::vec2, glm::vec3>(0);
}

gl::TriangleMesh::TriangleMesh(const std::vector<Eigen::Vector3f>& vertices, std::vector<Eigen::Vector3i>& indices) :
	gl::TriangleMesh()
{
	for (const Eigen::Vector3f& p : vertices) {
		glm::vec3 _p(p.x(), p.y(), p.z());
		mVertexData->push_back(_p, glm::vec2(0), glm::vec3(1, 0, 0));
	}
	gl::IndexBuffer& indexBuffer = getIndexBuffer();
	indexBuffer.resize(indices.size() * 3);
	for (size_t i = 0; i < indices.size(); ++i) {
		indexBuffer(3 * i) = indices[i].x();
		indexBuffer(3 * i + 1) = indices[i].y();
		indexBuffer(3 * i + 2) = indices[i].z();
	}
	computeNormals();
}

gl::TriangleMesh::TriangleMesh(const std::string& path) :
	TriangleMesh()
{
#ifndef WITH_ASSIMP
	throw std::runtime_error("Framework was not compiled with Assimp.");
#else
	std::cout << "Loading mesh from \"" << path << "\" ... ";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// Assume that we want to load the first mesh
	aiMesh* mesh = scene->mMeshes[0];
	mVertexData->resize(mesh->mNumVertices);
	#pragma omp parallel for
	for (int i = 0; i < (int)mesh->mNumVertices; ++i) {
		glm::vec3 vertex = reinterpret_cast<glm::vec3*>(mesh->mVertices)[i];
		glm::vec3 normal = mesh->HasNormals()
			? reinterpret_cast<glm::vec3*>(mesh->mNormals)[i]
			: glm::vec3(0);
		glm::vec2 uv = mesh->mTextureCoords[0]
			? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
			: glm::vec2(0);
		mVertexData->operator[](i) = std::make_tuple(vertex, uv, normal);
	}

	gl::IndexBuffer& indexBuffer = getIndexBuffer();
	indexBuffer.resize(mesh->mNumFaces * 3);
	#pragma omp parallel for
	for (int i = 0; i < (int)mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		indexBuffer[3 * i] = face.mIndices[0];
		indexBuffer[3 * i + 1] = face.mIndices[1];
		indexBuffer[3 * i + 2] = face.mIndices[2];
	}
	std::cout << "Done!\n";
#endif
}

void gl::TriangleMesh::render(const gl::RendererBase * env)
{
	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;

	Mesh::render(
		visualizeNormals ? mNormalShader : mShader,
		"MVP", MVP,
		"M", ModelMatrix,
		"color", mColor);
}

void gl::TriangleMesh::drawOutliner()
{
	ImGui::Text("Vertices %d| Faces %d", (int)numVertices(), (int)numFaces());
	ImGui::Checkbox("Visualize Normals", &visualizeNormals);
	ImGui::ColorEdit4("Surface color", &mColor.x);
}

void gl::TriangleMesh::addTriangles(std::vector<Eigen::Vector3f>& vertices)
{
	assert(vertices.size() % 3 == 0);
	unsigned int i0 = mVertexData->size();
	for(int i = 0; i < vertices.size(); ++i) {
		mVertexData->push_back(
			glm::vec3(vertices[i].x(), vertices[i].y(), vertices[i].z()),
			glm::vec2(0),
			glm::vec3(0));
		mBatch.indexBuffer->push_back(i0 + i);
	}
}

bool gl::TriangleMesh::handleIO(const Renderer* env, ImGuiIO& io)
{
	bool updated = false;
	//for (auto& vertex : *mVertices) {
	//	updated |= ImGui3D::Vertex(vertex.data());
	//}
	//mVertices->setDirty(updated);
	return updated;
}

void gl::TriangleMesh::computeNormals()
{
	std::vector<glm::vec3> vertexNormals(mVertexData->size(), glm::vec3(0));
	gl::IndexBuffer& indexBuffer = getIndexBuffer();
	for (int i = 0; i < indexBuffer.size(); i += 3) {
		const int i0 = indexBuffer[i];
		const int i1 = indexBuffer[i + 1];
		const int i2 = indexBuffer[i + 2];
		const glm::vec3 p0 = mVertexData->at<0>(i0);
		const glm::vec3 p1 = mVertexData->at<0>(i1);
		const glm::vec3 p2 = mVertexData->at<0>(i2);
		const glm::vec3 n = glm::cross(p1 - p0, p2 - p0);
		vertexNormals[i0] += n;
		vertexNormals[i1] += n;
		vertexNormals[i2] += n;
	}

	for (int i = 0; i < (int)vertexNormals.size(); ++i) {
		mVertexData->get<2>(i) = glm::normalize(vertexNormals[i]);
	}
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

gl::IndexBuffer& gl::Mesh::getIndexBuffer()
{
	return *mBatch.indexBuffer;
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

void gl::CoordinateFrame::render(const gl::RendererBase * env)
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


void gl::Plane::render(const gl::RendererBase * env)
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
