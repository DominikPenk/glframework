#pragma once

#include <memory>

#include "glpp/meshes/mesh.hpp"
#include "glpp/openmesh_ext.hpp"

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
		bool visualizeNormals;
				
	private:
		bool dirty;

		OpenMesh::TriangleMesh3f mesh;

		std::shared_ptr<Shader> triangleShader;
		std::shared_ptr<Shader> normalShader;
		std::shared_ptr<gl::PositionUVNormalBuffer3f> mVertexData;
	};

}