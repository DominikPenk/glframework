#pragma once
#include "mesh.hpp"

#include <glm/glm.hpp>

struct ImGuiIO;

namespace gl {
	class Renderer;

	class CatmullRomSpline : public Mesh {
	public:
		enum class EndpointCondition {
			Natural = 0,
			Periodic = 1,
			Clamped = 2,
			KnotAKnot = 3
		};

		CatmullRomSpline(const std::vector<glm::vec3>& points);

		virtual void render(const Renderer* env) override;

		virtual void drawOutliner() override; 

		virtual bool handleIO(const Renderer* env, ImGuiIO& io) override;

		glm::vec4 color;
		glm::vec3 q0, qk;
		int subdivisions;
		int linewidth;
		EndpointCondition endpointCondition;

	private:
		std::shared_ptr<VertexBufferObject<float, 3>> mPoints;
		VertexBufferObject<unsigned int, 1> mIndices;
		VertexArrayObject mVAO;
	};

}