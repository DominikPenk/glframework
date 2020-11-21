#include "glpp/meshes/triangle_mesh.hpp"

#include "glpp/renderer.hpp"

#include <glm/gtx/matrix_cross_product.hpp>

#ifdef WITH_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

gl::TriangleMesh::TriangleMesh() :
	Mesh(),
	visualizeNormals(false)
{
	mColor = glm::vec4(0.7f, 0.8f, 0.7f, 1.0f);

	mShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	mNormalShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle_normal.glsl");

	mVertexData = mBatch.addVertexAttributes<glm::vec3, glm::vec2, glm::vec3>(0);
}

gl::TriangleMesh::TriangleMesh(const std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& indices) :
	gl::TriangleMesh()
{
	for (const glm::vec3& p : vertices) {
		mVertexData->push_back(p, glm::vec2(0), glm::vec3(1, 0, 0));
	}
	gl::IndexBuffer& indexBuffer = getIndexBuffer();
	indexBuffer.resize(indices.size() * 3);
	for (size_t i = 0; i < indices.size(); ++i) {
		indexBuffer(3 * i) = indices[i].x;
		indexBuffer(3 * i + 1) = indices[i].y;
		indexBuffer(3 * i + 2) = indices[i].z;
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

void gl::TriangleMesh::render(const gl::RendererBase* env)
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

void gl::TriangleMesh::addTriangles(std::vector<glm::vec3>& vertices)
{
	assert(vertices.size() % 3 == 0);
	unsigned int i0 = mVertexData->size();
	for (int i = 0; i < vertices.size(); ++i) {
		mVertexData->push_back(
			vertices[i],
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