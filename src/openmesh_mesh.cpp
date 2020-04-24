#include "openmesh_mesh.h"

#include "renderer.hpp"

gl::OpenMeshMesh::OpenMeshMesh() :
	Mesh(),
	dirty(true),
	faceColor(.7f, .7f, .7f, 1.f),
	edgeColor(.1f, .1f, .1f, 1.f),
	vertexColor(.1f, .1f, .1f, 1.f),
	drawEdges(false)
{
	batch.shader = Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "triangle.glsl");
	vertices = batch.addVertexAttribute<float, 3>(0);
}

gl::OpenMeshMesh::OpenMeshMesh(OpenMesh::TriangleMesh3f mesh) : 
	OpenMeshMesh() {
	this->mesh = mesh;
}

void gl::OpenMeshMesh::render(const Renderer* env)
{
	update();
	glm::mat4 P = env->camera()->GetProjectionMatrix();
	glm::mat4 V = env->camera()->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;

	batch.execute("MVP", MVP, "color", faceColor);

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
