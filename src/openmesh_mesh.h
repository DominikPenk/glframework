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
		virtual void render(const Renderer* env) override;

		virtual void drawOutliner() override;

		OpenMesh::TriangleMesh3f& data();
		const OpenMesh::TriangleMesh3f& data() const;

		void update(bool force = false);

		glm::vec4 faceColor, edgeColor, vertexColor;
		bool drawVertices;
		bool drawEdges;

	private:
		bool dirty;
		OpenMesh::TriangleMesh3f mesh;

		// We can either use a pointer or construct a mesh directly ins
		DrawBatch batch;
		std::shared_ptr<VertexBufferObject<float, 3>> vertices;
	};

}