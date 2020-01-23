#pragma once
#include <glad/glad.h>

#include "shadermanager.hpp"
#include "buffers.hpp"
#include "camera.hpp"
#include "draw_batch.hpp"

#include <opencv2/core.hpp>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <eigen3/Eigen/Core>


class Heightmap;
struct ImGuiIO;


namespace gl {
	class Renderer;

	class Mesh {
	public:
		friend class OutlinerWindow;

		Mesh();
		virtual void render(const Renderer* env) = 0;

		Shader& setShader(std::string path) {
			mShader = Shader(path);
			return mShader;
		}
		Shader& setShader(Shader shader) {
			mShader = shader;
			return mShader;
		}

		Shader& getShader() { return mShader; }
		const Shader& getShader() const { return mShader; }

		/// <summary>Override this function if you want to display additional information about your object in the outliner</summary>
		virtual void drawOutliner();

		virtual bool handleIO(const Renderer* env, ImGuiIO& io) { return false; }

		/// <summary>Use this handle to draw either ImGui3D things or draw custom data</summary>
		/// <remarks>Framebuffer Attachment 0 is for display and Framebuffer Attachment 1 holds ImGuiIDs for io</remarks>
		virtual void drawViewportUI(const Renderer* env) { };

		std::string name;
		bool visible;
		glm::mat4 ModelMatrix;

	protected:
		bool mShowInOutliner;
		Shader mShader;
	};

	class TriangleMesh : public Mesh {
	public:
		TriangleMesh();
		TriangleMesh(const std::vector<Eigen::Vector3f>& vertices, std::vector<Eigen::Vector3i>& indices);

		void render(const Renderer* env);

		void addTriangles(std::vector<Eigen::Vector3f>& vertices);

		void removeDoubles(float thr = 1e-4f);

		void setColor(float r, float g, float b) { mColor = glm::vec4(r, g, b, 1); }

		size_t numPoints() const { return mVertices->size(); }

		virtual bool handleIO(const Renderer* env, ImGuiIO& io) override;

	protected:
		std::shared_ptr<VertexBufferObject<float, 3>> mVertices;
		DrawBatch mBatch;
		glm::vec4 mColor;
	};

	class CoordinateFrame : public Mesh {
	public:
		CoordinateFrame(float length = 10.0f);

		// Inherited via Mesh
		virtual void render(const Renderer* env) override;

		float axisLength;

	private:
		VertexBufferObject<float, 3> mPoints;
		VertexArrayObject mVAO;
	};

	class Plane : public Mesh {
	public:
		Plane(glm::vec3 position, glm::vec3 normal, glm::vec2 dimensions);

		virtual void render(const Renderer* env) override;

		glm::vec2 dimensions;
		glm::vec4 color;
	private:
		VertexArrayObject mVAO;
	};
}
