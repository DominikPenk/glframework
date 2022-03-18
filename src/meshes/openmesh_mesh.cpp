#include "glpp/meshes/openmesh_mesh.hpp"
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "glpp/renderer.hpp"

#include "../shaders/triangle.glsl.h"
#include "../shaders/triangle_normal.glsl.h"

gl::OpenMeshMesh::OpenMeshMesh() :
	Mesh(),
	dirty(true),
	faceColor(.7f, .7f, .7f, 1.f),
	edgeColor(.1f, .1f, .1f, 1.f),
	vertexColor(.1f, .1f, .1f, 1.f),
	drawEdges(false),
	visualizeNormals(false)
{
	//mShader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	//normalShader = std::make_shared<Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle_normal.glsl");
	mShader = Shader(std::initializer_list<std::pair<GLenum, std::string>>{
		{ GL_VERTEX_SHADER, TRIANGLE_VS },
		{ GL_FRAGMENT_SHADER, TRIANGLE_FS }});
	normalShader = std::make_shared<Shader>(std::initializer_list<std::pair<GLenum, std::string>>{
		{ GL_VERTEX_SHADER, TRIANGLE_NORMAL_VS },
		{ GL_FRAGMENT_SHADER, TRIANGLE_NORMAL_FS }});
	mVertexData = mBatch.addVertexAttributes<glm::vec3, glm::vec2, glm::vec3>(0);
}

gl::OpenMeshMesh::OpenMeshMesh(OpenMesh::TriangleMesh3f mesh) : 
	OpenMeshMesh() {
	this->mesh = mesh;
	this->mesh.request_vertex_normals();
	if (!mesh.has_vertex_normals())
	{
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
		return;
	}
	// let the mesh update the normals
	computeVertexNormals();
}

gl::OpenMeshMesh::OpenMeshMesh(std::string path) :
	OpenMeshMesh()
{
	this->mesh.request_vertex_normals();
	if (!OpenMesh::IO::read_mesh(mesh, path)) {
		std::cerr << "Could not load file: \"" << path << "\"\n";
	}
	else {
		std::printf("Loaded a mesh with %d vertices and %d faces\n", (int)mesh.n_vertices(), (int)mesh.n_faces());
		if (!mesh.has_vertex_normals())
		{
			std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
			return;
		}
		// let the mesh update the normals
		computeVertexNormals();
	}
}

void gl::OpenMeshMesh::render(const std::shared_ptr<gl::Camera> camera)
{
	update();
	glm::mat4 P = camera->GetProjectionMatrix();
	glm::mat4 V = camera->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;
	glDisable(GL_BLEND);
	
	Mesh::render(mShader,
		"MVP", MVP,
		"M", ModelMatrix,
		"color", faceColor);


	if (drawEdges) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.f, 1.f);
		Mesh::render(mShader,
			"MVP", MVP, 
			"color", edgeColor);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void gl::OpenMeshMesh::drawOutliner()
{
	ImGui::Checkbox("Draw Wireframe", &drawEdges);
	ImGui::Checkbox("Visualize Normals", &visualizeNormals);
	if (ImGui::TreeNode("Color")) {
		ImGui::ColorPicker4("Surface color", &faceColor.x);
		ImGui::ColorPicker4("Wireframe color", &edgeColor.x);
		ImGui::TreePop();
	}
}

OpenMesh::TriangleMesh3f& gl::OpenMeshMesh::data()
{
	dirty = true;
	return mesh;
}

const OpenMesh::TriangleMesh3f& gl::OpenMeshMesh::data() const
{
	return mesh;
}

void gl::OpenMeshMesh::update(bool force)
{
	if (dirty || force) {
		// Copy vertices
		mVertexData->resize(mesh.n_vertices());
		for (size_t i = 0; i < mesh.n_vertices(); ++i) {
			glm::vec3 p = mesh.points()[i];
			glm::vec3 n = mesh.vertex_normals()[i];
			mVertexData->at(i) = { p, glm::vec2(0), n };
		}
		IndexBuffer& indexBuffer = *mBatch.indexBuffer;
		indexBuffer.reserve(3 * mesh.n_faces());
		for (auto face : mesh.faces()) {
			auto heh = face.halfedge();
			indexBuffer.push_back(heh.from().idx());
			indexBuffer.push_back(heh.to().idx());
			indexBuffer.push_back(heh.next().to().idx());
		}
		dirty = false;
	}
}

void gl::OpenMeshMesh::computeVertexNormals()
{
	for (auto vh : mesh.vertices()) {
		glm::vec<3, float> n;
		mesh.calc_vertex_normal_correct(vh, n);
		mesh.set_normal(vh, n);
	}
	mesh.update_normals();
	dirty = true;
}