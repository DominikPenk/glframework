#pragma once

#include <memory>

#include "mesh.hpp"
#include "openmesh_ext.h"

namespace gl {


	class OpenMeshMesh : public Mesh {
	public:
		OpenMeshMesh();
		OpenMeshMesh(OpenMesh::TriangleMesh3f);

		// Inherited via Mesh
		virtual void render(const RendererBase* env) override;

		virtual void drawOutliner() override;

		OpenMesh::TriangleMesh3f& data();
		const OpenMesh::TriangleMesh3f& data() const;

		void update(bool force = false);

		glm::vec4 faceColor, edgeColor, vertexColor;
		bool drawEdges;

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

		virtual Shader& getShader() override { return batch.shader; }
		virtual const Shader& getShader() const { return batch.shader; }

		DrawBatch& getBatch() {
			return batch;
		}

	private:
		bool dirty;
		OpenMesh::TriangleMesh3f mesh;

		// We can either use a pointer or construct a mesh directly ins
		DrawBatch batch;
		std::shared_ptr<VertexBufferObject<float, 3>> vertices;
	};

}