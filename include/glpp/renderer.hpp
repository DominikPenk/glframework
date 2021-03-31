#pragma once

#include <functional>

namespace gl {

	class ViewportEditorWindow;

	enum class ToneMapping {
		Linear                     = 0,
		Reinhard                   = 1,
		HaarmPeterDuiker           = 2,
		JimHejlRicharBurgessDawson = 3,
		Uncharted2                 = 4
	};

	enum class RenderHook {
		PreMeshDrawing,
		PostMeshDrawing,
		PostToneMapping,
		PreImGui3D,
		PostImGui3D,
		ImGuiDrawing
	};

	typedef typename std::function<void(const gl::ViewportEditorWindow*)> RenderHookFn;

#if false
	class Renderer : public RendererBase {
	public:


		Renderer(int width, int height, std::shared_ptr<Camera> cam, const std::string& title = "Title", bool maximized = false);

		

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

		PointLight& light() { return mLight; }
		PointLight light() const { return mLight; }

		virtual std::shared_ptr<Camera> camera() override { return mCamera; }
		virtual const std::shared_ptr<Camera> camera() const override { return mCamera; }

		size_t numMeshes() const { return mMeshes.size(); }

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

		void resizeBuffers(int width, int height) {
			mPrePostroBuffer->resize(width, height);
			mFrameBuffer->resize(width, height);
		}

		std::mutex& getLock(std::shared_ptr<Mesh> obj);
		std::mutex& getLock(std::shared_ptr<UIWindow> window);
	
	protected:
		friend class RendererDebugWindow;
		friend class OutlinerWindow;
		std::shared_ptr<Camera> mCamera;

	private:
		void ImGui3d_ImplRenderer_Init(std::shared_ptr<ImGui3D::ImGui3DContext> ctx);

		std::unique_ptr<UIWindow> mOutliner;
		std::unique_ptr<UIWindow> mDebugWindow;

		// Used prior to 
		std::shared_ptr<Framebuffer> mPrePostroBuffer;
		std::shared_ptr<Framebuffer> mFrameBuffer;

		std::vector<std::shared_ptr<Mesh>> mMeshes;

		std::vector<std::shared_ptr<UIWindow>> mUIWindows;
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
#endif
}