#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include <glpp/renderer.hpp>
#include <glpp/renderers/editorwindow.hpp>

namespace gl {

	class Camera;
	class Control;
	class Framebuffer;
	class GLFWContext;
	class Mesh;
	class Shader;

	typedef unsigned int EditorFlags;

	enum EditorFlags_ {
		EditorFlags_None              = 0x0,
		EditorFlags_NoDefaultOutliner = 0x1 << 1,
		EditorFlags_NoDefaultDebug    = 0x1 << 2,
		EditorFlags_NoDefaultLogging  = 0x1 << 3,
		EditorFlags_NoDefaultViewport = 0x1 << 3
	};

	class Editor {
	public:

		

		Editor();
		Editor(int width, int height, const std::string& title = "Title", EditorFlags flags = 0);
		Editor(const std::string& title, EditorFlags flags = 0);

		/// <summary>This function initializes a new frame and polls for user input.</summary>
		/// <remarks>Call this function as soon as possible in your render loop. 
		/// In most cases you should call this first.</remarks>
		/// <returns>True if the window will be drawn or false otherwise (e.g. if window is iconyfied)</returns>
		bool startFrame();

		/// <summary>This function finalizes the frame by drawing UI and geometry.</summary>
		/// <remarks>Call this function as late as possbile in your render loop. In most cases this is the last function called.</remarks>
		void endFrame();

		/// <summary>
		/// This function runs the default behaviour of the Editor (calling startFrame/endFrame in a loop until context is closed).
		/// If you need additional functionality you can do this yourself like this:
		/// ...
		/// Editor editor(...);
		/// while(!editor.should_close()) {
		///		startFrame();
		///		...
		///		endFrame();
		/// }
		/// </summary>
		void run();

		bool shouldClose() const;

		size_t addObject(const std::string& name, std::shared_ptr<Mesh> mesh);

		template<typename MeshType, typename ...Args>
		std::shared_ptr<MeshType> addObject(const std::string& name, Args ...args) {
			static_assert(std::is_base_of_v<gl::Mesh, MeshType>, "MeshType must derive from gl::Mesh");
			std::shared_ptr<MeshType> mesh = std::make_shared<MeshType>(args...);
			addObject(name, mesh);
			return mesh;
		}

		// Getters
		const std::vector<std::shared_ptr<Mesh>>& getObjects() const;
		std::vector<std::shared_ptr<Mesh>>        getObjects();

		std::shared_ptr<ViewportEditorWindow>     getViewport(int id = 0) const;
		template<typename WindowType>
		std::vector<std::shared_ptr<WindowType>>  getEditorWindows() const {
			static_assert(std::is_base_of_v<gl::EditorWindow, WindowType>, "WindowType must derive from gl::WindowType");
			std::vector<std::shared_ptr<WindowType>> windows;
			for (auto window : mEditorWindows) {
				if (std::dynamic_pointer_cast<WindowType>(window) != nullptr) {
					windows.push_back(std::dynamic_pointer_cast<WindowType>(window));
				}
			}
			return windows;
		}

		/// <summary>
		/// Returns the camera of the requested viewport.
		/// This is a shorthand for editor.getViewport(id).camera;
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		std::shared_ptr<gl::Camera>                   getViewportCamera(int id = 0) const;
		/// <summary>
		/// Returns the camera controls of the requested viewport.
		/// This is a shorthand for editor.getViewport(id).camera;
		/// </summary>
		/// <param name="id"></param>
		/// <returns></returns>
		std::shared_ptr<gl::Control>                   getViewportControls(int id = 0) const;


		void resetUILayout();

		// Public variables (mainly influencing ui stuff)
		bool showOutliner;
		bool showDebug;
		bool gammaCorrection;
		bool hdr;
		float gamma;
		ToneMapping toneMapping;
		glm::vec4 clearColor;

	protected:
		virtual unsigned int buildDefaultLayout(bool force);

		friend class DebugEditorWindow;
		std::shared_ptr<GLFWContext> mContext;

		std::vector<std::shared_ptr<Mesh>>         mObjects;
		std::vector<std::shared_ptr<EditorWindow>> mEditorWindows;

	private:
		/// <summary>
		/// This function initializes ImGui and ImGui3D
		/// </summary>
		void initialize(EditorFlags flags);

	private:
		bool                    mForceUiResetOnNextDraw;
		//ToneMapping             mLastTonemapping;
		//std::unique_ptr<Shader> mTonemappingShader;

	};


}