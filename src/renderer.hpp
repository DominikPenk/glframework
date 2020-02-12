#pragma once

#include "camera.hpp"
#include "mesh.hpp"

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "framebuffer.hpp"


namespace ImGui3D {
	struct ImGui3DContext;
}

namespace gl {
	class UIWindow;
	class GenericUIWindow;

	struct PointLight {
		PointLight() {
			position = glm::fvec3(0, 0, 0);
			color = glm::fvec3(1, 1, 1);
		}
		glm::fvec3 position;
		glm::fvec3 color;
	};

	class Renderer {
	public:

		enum class ToneMapping {
			Linear                     = 0,
			Reinhard                   = 1,
			HaarmPeterDuiker           = 2,
			JimHejlRicharBurgessDawson = 3,
			Uncharted2                 = 4
		};

		Renderer(int width, int height, std::shared_ptr<Camera> cam, const std::string& title = "Title", bool maximized = false);
		~Renderer();

		/// <summary>This function initializes a new frame and polls for user input.</summary>
		/// <remarks>Call this function as soon as possible in your render loop. 
		/// In most cases you should call this first.</remarks>
		/// <returns>True if the window will be drawn or false otherwise (e.g. if window is iconyfied)</returns>
		bool startFrame();

		/// <summary>This function finalizes the frame by drawing UI and geometry.</summary>
		/// <remarks>Call this function as late as possbile in your render loop. In most cases this is the last function called.</remarks>
		void endFrame();

		/// <summary>Construct a new window and add it to the ui.</summary>
		/// <param name="args">Parameters for the constructor of the new window.</param>
		/// <returns>A shared pointer to the newly constructed ui window.</returns>
		template<typename WindowType, typename ...Args>
		std::shared_ptr<WindowType> addUIWindow(Args ...args) {
			static_assert(std::is_base_of_v<UIWindow, WindowType>, "Tried to add window not derived from UIWindow");
			std::shared_ptr<WindowType> window = std::make_shared<WindowType>(args...);
			addUIWindow(window);
			return window;
		}

		/// <summary>Add an existing window to the ui.</summary>
		/// <param name="window">Window to add to the ui.</param>
		void addUIWindow(std::shared_ptr<UIWindow> window);

		/// <summary>Add a new ui window to the ui using a drafunction.</summary>
		/// <param name="title">Title of the new window.</param>
		/// <param name="drawFn">Function drawing the window contents.</param>
		/// <returns>A shared pointer to the newly constructed ui window.</returns>
		std::shared_ptr<GenericUIWindow> addUIWindow(std::string title, std::function<void(Renderer*)> drawFn);

		std::shared_ptr<Camera> camera() { return mCamera; }
		const std::shared_ptr<Camera> camera() const { return mCamera; }

		inline void setSize(size_t width, size_t height) {
			mCamera->ScreenHeight = static_cast<int>(height);
			mCamera->ScreenWidth  = static_cast<int>(width);
		}

		size_t addMesh(const std::string& name, std::shared_ptr<Mesh> mesh); 

		template<typename MeshType, typename ...Args>
		std::shared_ptr<MeshType> addMesh(const std::string& name, Args ...args) {
			std::shared_ptr<MeshType> mesh = std::make_shared<MeshType>(args...);
			addMesh(name, mesh);
			return mesh;
		}

		void removeMesh(size_t index) {
			mMeshes.erase(mMeshes.begin() + index);
		}

		inline const GLFWwindow* window() const { return mWindow; }

		void setTitle(const std::string& title);

		PointLight& light() { return mLight; }
		PointLight light() const { return mLight; }

		size_t numMeshes() const { return mMeshes.size(); }

		bool shouldClose() const { return glfwWindowShouldClose(mWindow); }
		bool isMinified() const { glfwGetWindowAttrib(mWindow, GLFW_ICONIFIED); }

		const std::vector<std::shared_ptr<Mesh>>& objects() const;
		std::vector<std::shared_ptr<Mesh>>& objects();

		const ToneMapping toneMapping() const { return mToneMapping; }
		void setToneMapping(ToneMapping mapping);

		bool showOutliner;
		bool showDebug;
		bool gammaCorrection;
		bool hdr;
		float gamma;

		void pushResizeCallback(std::function<void(Renderer*)> fn);
		std::vector<std::function<void(Renderer*)>> resizeCallbacks();

		std::shared_ptr<Framebuffer> framebuffer() { return mFrameBuffer; }

	private:
		void ImGui3d_ImplRenderer_Init(std::shared_ptr<ImGui3D::ImGui3DContext> ctx);

		int m_version[2];

		std::unique_ptr<UIWindow> mOutliner;
		std::unique_ptr<UIWindow> mDebugWindow;

		std::shared_ptr<Framebuffer> mFrameBuffer;

		std::vector<std::shared_ptr<Mesh>> mMeshes;
		std::shared_ptr<Camera> mCamera;

		std::vector<std::shared_ptr<UIWindow>> mUIWindows;
		glm::fvec4 mClearColor;
		GLFWwindow* mWindow;
		PointLight mLight;
		ToneMapping mToneMapping;
		gl::Shader mPostProShader;
		gl::Shader mDisplayShader;
		gl::VertexArrayObject mDummyVAO;

		std::vector<std::function<void(Renderer*)>> mResizeCallbacks;
	};

}