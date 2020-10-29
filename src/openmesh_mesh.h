#pragma once

#include <memory>

#include "mesh.hpp"
#include "openmesh_ext.h"

namespace gl {


	class OpenMeshMesh : public Mesh {
	public:
		OpenMeshMesh();
		OpenMeshMesh(OpenMesh::TriangleMesh3f);
		OpenMeshMesh(std::string path);

		// Inherited via Mesh
		virtual void render(const RendererBase* env) override;

		virtual void drawOutliner() override;

		OpenMesh::TriangleMesh3f& data();
		const OpenMesh::TriangleMesh3f& data() const;

		void update(bool force = false);

		void computeVertexNormals();

		glm::vec4 faceColor, edgeColor, vertexColor;
		bool drawEdges;
		bool displayNormals;

		void watch(OpenMesh::VertexHandle vh);
		void watch(OpenMesh::EdgeHandle eh);
		void watch(OpenMesh::HalfedgeHandle heh);
		void watch(OpenMesh::FaceHandle fh);

		void addBreakpoint(OpenMesh::VertexHandle vh,    const std::function<void()>& callback = []() { __debugbreak(); }, OpenMesh::Conditional condition = OpenMesh::Always);
		void addBreakpoint(OpenMesh::EdgeHandle eh,      const std::function<void()>& callback = []() { __debugbreak(); }, OpenMesh::Conditional condition = OpenMesh::Always);
		void addBreakpoint(OpenMesh::HalfedgeHandle heh, const std::function<void()>& callback = []() { __debugbreak(); }, OpenMesh::Conditional condition = OpenMesh::Always);
		void addBreakpoint(OpenMesh::FaceHandle fh,      const std::function<void()>& callback = []() { __debugbreak(); }, OpenMesh::Conditional condition = OpenMesh::Always); 
		
		void stopWatch(OpenMesh::VertexHandle vh);
		void stopWatch(OpenMesh::EdgeHandle eh);
		void stopWatch(OpenMesh::HalfedgeHandle heh);
		void stopWatch(OpenMesh::FaceHandle fh);

		void disableBreakPointCheck(bool value);

		virtual Shader& getShader() override { return *batch.shader; }
		virtual const Shader& getShader() const { return *batch.shader; }


		DrawBatch& getBatch() {
			return batch;
		}

	private:
		bool dirty;

		OpenMesh::TriangleMesh3f mesh;

		std::shared_ptr<Shader> triangleShader;
		std::shared_ptr<Shader> normalShader;
		DrawBatch batch;
		// We can either use a pointer or construct a mesh directly ins
		std::shared_ptr<VertexBufferObject<float, 3>> vertices;
		std::shared_ptr<VertexBufferObject<float, 3>> normals;
	};

}