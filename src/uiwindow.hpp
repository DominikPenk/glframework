#pragma once

#include <string>
#include <functional>

namespace gl {
	class Renderer;

	class UIWindow {
	public:
		UIWindow(std::string title);

		void draw(Renderer* env);
		virtual void preDraw(Renderer* env);
		virtual void viewportDraw(Renderer* env);

		std::string title;
		bool open;

	protected:
		virtual void onDraw(Renderer* env) = 0;
	};

	class GenericUIWindow : public UIWindow {
	public:
		GenericUIWindow(std::string title, std::function<void(Renderer*)> drawFn);

	protected:
		virtual void onDraw(Renderer* env) override;

	private:
		std::function<void(Renderer*)> mDrawFn;
	};

	class OutlinerWindow : public UIWindow {
	public:
		OutlinerWindow();

		virtual void onDraw(Renderer* env) override;
	};

	class RendererDebugWindow : public UIWindow {
	public:
		RendererDebugWindow();
	
		virtual void onDraw(Renderer* env) override;
	};

}