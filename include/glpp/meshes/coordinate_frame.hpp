#pragma once

#include "glpp/buffers.hpp"
#include "glpp/meshes/mesh.hpp"

namespace gl {
	class RendererBase;

	class CoordinateFrame : public Mesh {
	public:
		CoordinateFrame(float length = 10.0f);

		// Inherited via Mesh
		virtual void render(const RendererBase* env) override;

		float axisLength;

	private:
		VertexBufferObject<float, 3> mPoints;
		VertexArrayObject mVAO;
	};
}
