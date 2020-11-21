#pragma once
#include <glad/glad.h>

#include "glpp/shadermanager.hpp"
#include "glpp/buffers.hpp"
#include "glpp/camera.hpp"
#include "glpp/draw_batch.hpp"

#ifdef WITH_OPENMESH
#include "glpp/openmesh_ext.hpp"
#endif

#include <opencv2/core.hpp>
#include <fstream>
#include <sstream>
#include <unordered_set>

class Heightmap;
struct ImGuiIO;


namespace gl {
	class RendererBase;
	class Renderer;

	class Mesh {
	public:
		friend class OutlinerWindow;

		Mesh();
		virtual void render(const RendererBase * env) = 0;

		template<typename... Args>
		void render(gl::Shader& shader, const Args&... uniforms) {
			mBatch.execute(shader, uniforms...);
		}

		Shader& setShader(std::string path) {
			mShader = Shader(path);
			return mShader;
		}
		Shader& setShader(Shader shader) {
			mShader = shader;
			return mShader;
		}

		virtual Shader& getShader() { return mShader; }
		virtual const Shader& getShader() const { return mShader; }

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
		gl::IndexBuffer& getIndexBuffer();

		gl::DrawBatch mBatch;
		bool mShowInOutliner;
		Shader mShader;
	};

	class TriangleMesh : public Mesh {
	public:
		TriangleMesh();
		TriangleMesh(const std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& indices);

		TriangleMesh(const std::string& path);

		void render(const RendererBase* env);

		virtual void drawOutliner() override;

		void addTriangles(std::vector<glm::vec3>& vertices);

		void setColor(float r, float g, float b) { mColor = glm::vec4(r, g, b, 1); }

		size_t numVertices() const { return mVertexData->size(); }
		size_t numFaces() const { return mBatch.indexBuffer->size() / 3; }

		virtual bool handleIO(const Renderer* env, ImGuiIO& io) override;

		void computeNormals();

		bool visualizeNormals;
	protected:
		std::shared_ptr<gl::PositionUVNormalBuffer3f> mVertexData;
		glm::vec4 mColor;
		Shader mNormalShader;
	};

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

	class Plane : public Mesh {
	public:
		Plane(glm::vec3 position, glm::vec3 normal, glm::vec2 dimensions);

		virtual void render(const RendererBase* env) override;

		glm::vec2 dimensions;
		glm::vec4 color;
	private:
		VertexArrayObject mVAO;
	};
}
