#pragma once

#include "glpp/meshes/mesh.hpp"
#include <glm/glm.hpp>

namespace gl {
	class RendererBase;

	class TriangleMesh : public Mesh {
	public:
		TriangleMesh();
		TriangleMesh(const std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& indices);

		TriangleMesh(const std::string& path);

		void render(const std::shared_ptr<gl::Camera> camera);

		virtual void drawOutliner() override;

		void addTriangles(std::vector<glm::vec3>& vertices);

		void setColor(float r, float g, float b) { mColor = glm::vec4(r, g, b, 1); }

		size_t numVertices() const { return mVertexData->size(); }
		size_t numFaces() const { return mBatch.indexBuffer->size() / 3; }

		glm::vec3 vertex(size_t i) const;
		glm::vec3& vertex(size_t i);
		glm::vec4& color();

		std::tuple<unsigned int, unsigned int, unsigned int> face(size_t i) const;

		void transform(glm::mat4 T);

		virtual bool handleIO(const std::shared_ptr<gl::Camera> camera, ImGuiIO& io) override;

		void computeNormals();

		bool visualizeNormals;
	protected:
		std::shared_ptr<gl::PositionUVNormalBuffer3f> mVertexData;
		glm::vec4 mColor;
		Shader mNormalShader;
	};

}