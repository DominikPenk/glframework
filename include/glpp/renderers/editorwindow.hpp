#pragma once

#include <memory>
#include <string>
#include <deque>

#include <glpp/renderer.hpp>
#include <glpp/imgui.hpp>
#include <glpp/logging.hpp>

namespace ImGui3D {
	struct ImGui3DContext;
}

namespace gl {

	class Editor;
	class Framebuffer;
	class Shader;

	/// <summary>
	/// Enum identifying the position of a ui window
	/// </summary>
	enum class EditorWindowRegion {
		Left     = 0,
		Right    = 1,
		Bottom   = 2,
		Top      = 3,
		Center   = 4,
		Floating = 5,
	};

	class EditorWindow {
	public:
		EditorWindow(const std::string& title, EditorWindowRegion defaultRegion);

		void draw(Editor* editor);
		virtual void preDraw(Editor* editor);
		virtual void viewportDraw(Editor* editor);

		template<typename WindowType>
		inline bool is() const {
			static_assert(std::is_base_of_v<EditorWindow, WindowType>, "WindowType must derive from gl::EditorWindow");
			return dynamic_cast<const WindowType*>(this) != nullptr;
		}

		std::string title;
		bool open;
		EditorWindowRegion defaultRegion;

	protected:
		virtual void onDraw(Editor* editor) = 0;
		virtual void onResize(ImVec2 position, ImVec2 windowSize, Editor* editor);
		virtual void initialize(Editor* editor);

		ImVec2 position;
		ImVec2 size;
		bool initialized;
	};

	class OutlinerEditorWindow : public EditorWindow {
	public:
		OutlinerEditorWindow(const std::string& title = "Outliner", EditorWindowRegion defaultRegion = EditorWindowRegion::Right);

	protected:
		virtual void onDraw(Editor* editor) override;
	};

	class DebugEditorWindow : public EditorWindow {
	public:
		DebugEditorWindow(const std::string& title = "Debug", EditorWindowRegion defaultRegion = EditorWindowRegion::Left);

	protected:
		virtual void onDraw(Editor* editor) override;
	};

	class ViewportEditorWindow : public EditorWindow {
	public:
		ViewportEditorWindow(const std::string& title = "Viewport", EditorWindowRegion defaultRegion = EditorWindowRegion::Center);

	protected:
		friend class DebugEditorWindow;
		void onDraw(Editor* editor) override;
		void onResize(ImVec2 position, ImVec2 windowSize, Editor* editor) override;
		void initialize(Editor* editor) override;

		ToneMapping                              mLastTonemapping;
		std::shared_ptr<Framebuffer>             mGeometryFrameBuffer;
		std::shared_ptr<Framebuffer>             mFrameBuffer;
		std::unique_ptr<Shader>                  mTonemappingShader;
		std::shared_ptr<ImGui3D::ImGui3DContext> mImGui3DContext;

	private:
		std::shared_ptr<ImGui3D::ImGui3DContext> mOldImGui3DContext;
	};

	class LoggingEditorWindow : public EditorWindow, public LoggingEndpoint {
	public:
		LoggingEditorWindow(const std::string& title = "Messages", EditorWindowRegion defaultRegion = EditorWindowRegion::Bottom);
		~LoggingEditorWindow();

		int maxLogSize;

		virtual void onMessage(const LogMessage& msg) override;

		std::string icons[4];

	
	protected:
		void onDraw(Editor* editor);

		std::deque<std::pair<int, std::string>> mLogs;
		bool mEnabled[4];
		bool mMessageJustLogged;
	};
}