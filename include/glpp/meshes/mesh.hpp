#pragma once 

#include <glm/glm.hpp>

#include "glpp/buffers.hpp"
#include "glpp/shadermanager.hpp"
#include "glpp/draw_batch.hpp"
#include <glpp/camera.hpp>

namespace gl {

	class Mesh {
	public:
		friend class OutlinerEditorWindow;

		Mesh();
		virtual void render(const std::shared_ptr<gl::Camera> camera) = 0;

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

		virtual bool handleIO(const std::shared_ptr<gl::Camera> env, ImGuiIO& io) { return false; }

		/// <summary>Use this handle to draw either ImGui3D things or draw custom data</summary>
		/// <remarks>Framebuffer Attachment 0 is for display and Framebuffer Attachment 1 holds ImGuiIDs for io</remarks>
		virtual void drawViewportUI(const std::shared_ptr<gl::Camera> env) { };

		std::string name;
		bool visible;
		glm::mat4 ModelMatrix;

	protected:
		gl::IndexBuffer& getIndexBuffer();

		gl::DrawBatch mBatch;
		bool mShowInOutliner;
		Shader mShader;
	};
}