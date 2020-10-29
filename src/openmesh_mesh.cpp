#include "openmesh_mesh.h"
#include <OpenMesh/Core/IO/MeshIO.hh>

#include "renderer.hpp"

gl::OpenMeshMesh::OpenMeshMesh() :
	Mesh(),
	dirty(true),
	faceColor(.7f, .7f, .7f, 1.f),
	edgeColor(.1f, .1f, .1f, 1.f),
	vertexColor(.1f, .1f, .1f, 1.f),
	drawEdges(false),
	displayNormals(false)
{
	batch.shader = std::make_shared<Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	triangleShader = batch.shader;
	normalShader = std::make_shared<Shader>(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle_normal.glsl");
	vertices = batch.addVertexAttribute<float, 3>(0);
	normals = batch.addVertexAttribute<float, 3>(1);
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

void gl::OpenMeshMesh::render(const gl::RendererBase * env)
{
	update();
	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;
	glDisable(GL_BLEND);
	if (displayNormals) {
		batch.shader = normalShader;
		batch.execute(
			"MVP", MVP,
			"M", ModelMatrix);
	}
	else {
		batch.shader = triangleShader;
		batch.execute("MVP", MVP, "color", faceColor);
	}


	if (drawEdges) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.f, 1.f);
		batch.execute("MVP", MVP, "color", edgeColor);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void gl::OpenMeshMesh::drawOutliner()
{
	ImGui::Checkbox("Draw Wireframe", &drawEdges);
	ImGui::Checkbox("Visualize Normals", &displayNormals);
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
		vertices->resize(mesh.n_vertices());
		std::copy(mesh.points(), mesh.points() + mesh.n_vertices(), reinterpret_cast<glm::vec3*>(vertices->data()));
		// Copy normals
		normals->resize(mesh.n_vertices());
		std::copy(mesh.vertex_normals(), mesh.vertex_normals() + mesh.n_vertices(), reinterpret_cast<glm::vec3*>(normals->data()));
		IndexBuffer& indexBuffer = *batch.indexBuffer;
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

void gl::OpenMeshMesh::watch(OpenMesh::VertexHandle vh)
{
#ifdef _DEBUG
	mesh.watch(vh);
#endif
}

void gl::OpenMeshMesh::watch(OpenMesh::EdgeHandle eh)
{
#ifdef _DEBUG
	mesh.watch(eh);
#endif
}

void gl::OpenMeshMesh::watch(OpenMesh::HalfedgeHandle heh)
{
#ifdef _DEBUG
	mesh.watch(heh);
#endif
}

void gl::OpenMeshMesh::watch(OpenMesh::FaceHandle fh)
{
#ifdef _DEBUG
	mesh.watch(fh);
#endif
}

void gl::OpenMeshMesh::addBreakpoint(OpenMesh::VertexHandle vh, const std::function<void()>& callback, OpenMesh::Conditional condition)
{
#ifdef _DEBUG
	mesh.addBreakpoint(vh, callback, condition);
#endif
}

void gl::OpenMeshMesh::addBreakpoint(OpenMesh::EdgeHandle eh, const std::function<void()>& callback, OpenMesh::Conditional condition)
{
#ifdef _DEBUG
	mesh.addBreakpoint(eh, callback, condition);
#endif
}

void gl::OpenMeshMesh::addBreakpoint(OpenMesh::HalfedgeHandle heh, const std::function<void()>& callback, OpenMesh::Conditional condition)
{
#ifdef _DEBUG
	mesh.addBreakpoint(heh, callback, condition);
#endif
}

void gl::OpenMeshMesh::addBreakpoint(OpenMesh::FaceHandle fh, const std::function<void()>& callback, OpenMesh::Conditional condition)
{
#ifdef _DEBUG
	mesh.addBreakpoint(fh, callback, condition);
#endif
}

void gl::OpenMeshMesh::stopWatch(OpenMesh::VertexHandle vh)
{
#ifdef _DEBUG
	mesh.stopWatch(vh);
#endif
}

void gl::OpenMeshMesh::stopWatch(OpenMesh::EdgeHandle eh)
{
#ifdef _DEBUG
	mesh.stopWatch(eh);
#endif
}

void gl::OpenMeshMesh::stopWatch(OpenMesh::HalfedgeHandle heh)
{
#ifdef _DEBUG
	mesh.stopWatch(heh);
#endif
}

void gl::OpenMeshMesh::stopWatch(OpenMesh::FaceHandle fh)
{
#ifdef _DEBUG
	mesh.stopWatch(fh);
#endif
}

void gl::OpenMeshMesh::disableBreakPointCheck(bool value)
{
#ifdef _DEBUG
	mesh.disableCheck = value;
#endif
}
