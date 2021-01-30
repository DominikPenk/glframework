#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace ImGui3D {
	struct ImGui3DContext;
}


namespace gl {

	class Camera;
	class Control;
	class Context;
	class Framebuffer;
	class GLFWContext;
	class Mesh;

	class ImmediateRenderer {
	public:
		ImmediateRenderer();
		ImmediateRenderer(int width, int height, const std::string& title, std::shared_ptr<Context> shared = nullptr);
		ImmediateRenderer(const std::string& title, std::shared_ptr<Context> shared = nullptr);

		bool startFrame();
		void endFrame();
		bool shouldClose() const;

		void renderObjects(std::shared_ptr<Camera>) const;

		size_t addObject(std::shared_ptr<Mesh> mesh);
		template<typename MeshType, typename ...Args>
		std::shared_ptr<MeshType> addObject(Args ...args) {
			static_assert(std::is_base_of_v<gl::Mesh, MeshType>, "MeshType must derive from gl::Mesh");
			std::shared_ptr<MeshType> mesh = std::make_shared<MeshType>(args...);
			addObject(mesh);
			return mesh;
		}

		glm::vec4                              clearColor;
		std::vector<std::shared_ptr<gl::Mesh>> objects;
		std::shared_ptr<gl::Camera>            viewportCamera;
		std::shared_ptr<gl::Control>           viewportControl;

	protected:
		std::shared_ptr<GLFWContext> mContext;
		std::shared_ptr<Framebuffer> mFrameBuffer;

	private:
		void initialize();
		std::shared_ptr<ImGui3D::ImGui3DContext> mOldImGui3DContext;
		std::shared_ptr<ImGui3D::ImGui3DContext> mImGui3DContext;
	};


}