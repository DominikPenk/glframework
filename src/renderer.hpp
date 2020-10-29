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

	class RendererBase {
	public:
		RendererBase(std::shared_ptr<Camera> cam);
		std::shared_ptr<Camera> camera() { return mCamera; }
		const std::shared_ptr<Camera> camera() const { return mCamera; }
	protected:
		GLFWwindow* mWindow;
		std::shared_ptr<Camera> mCamera;
		int mVersion[2];
	};

	class Renderer : public RendererBase {
	public:
		enum class RenderHook {
			Pre2DGui,
			PostMeshDrawing
		};

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

		template<class MeshType>
		std::vector<std::shared_ptr<MeshType>> getMeshes() {
			std::vector<std::shared_ptr<MeshType>> filtered;
			for (std::shared_ptr<Mesh> mesh : mMeshes) {
				std::shared_ptr<MeshType> converted = std::dynamic_pointer_cast<MeshType, gl::Mesh>(mesh);
				if (converted != nullptr) {
					filtered.push_back(converted);
				}
			}
			return filtered;
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

		void addRenderHook(RenderHook hookType, std::function<void(Renderer*)> hook);

		bool showOutliner;
		bool showDebug;
		bool showMeshWatch;
		bool gammaCorrection;
		bool hdr;
		float gamma;
		glm::vec4 clearColor;

		void pushResizeCallback(std::function<void(Renderer*)> fn);
		std::vector<std::function<void(Renderer*)>> resizeCallbacks();

		std::shared_ptr<Framebuffer> framebuffer() { return mFrameBuffer; }

		std::mutex& getLock(std::shared_ptr<Mesh> obj);
		std::mutex& getLock(std::shared_ptr<UIWindow> window);

	private:
		void ImGui3d_ImplRenderer_Init(std::shared_ptr<ImGui3D::ImGui3DContext> ctx);

		std::unique_ptr<UIWindow> mOutliner;
		std::unique_ptr<UIWindow> mDebugWindow;
		std::unique_ptr<UIWindow> mMeshWatch;

		std::shared_ptr<Framebuffer> mFrameBuffer;

		std::vector<std::shared_ptr<Mesh>> mMeshes;

		std::vector<std::shared_ptr<UIWindow>> mUIWindows;
		PointLight mLight;
		ToneMapping mToneMapping;
		gl::Shader mPostProShader;
		gl::Shader mDisplayShader;
		gl::VertexArrayObject mDummyVAO;

		std::vector<std::function<void(Renderer*)>> mResizeCallbacks;

		/// <summary>
		/// This hook is called just before ImGui is rendered and 
		// after prosprocessed Render content and ImGui3D content was drawn.
		/// </summary>
		std::vector<std::function<void(Renderer*)>> mPreImGuiHook;
		std::vector<std::function<void(Renderer*)>> mPostMeshDrawingHook;

		// In case of threading
		std::mutex mLockLock;
		std::unordered_map<std::shared_ptr<Mesh>, std::mutex> mObjectLocks;
		std::unordered_map<std::shared_ptr<UIWindow>, std::mutex> mWindowLocks;
	};

}