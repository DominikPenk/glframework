#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace gl {

	class Renderer;
	class Sprite;

	class CanvasElement {
	public:
		// Interaction return values
		static constexpr int PASS = 0x00;
		static constexpr int STOP = 0x01;
		static constexpr int START_DRAG = 0x03;

		CanvasElement();
		virtual void draw(int x, int y, int layers) = 0;
		virtual bool overlaps(int x, int y) const = 0;

		virtual int onMouseDown(int x, int y) { return PASS; }
		virtual int onMouseUp(int x, int y) { return PASS;  }
		virtual int onMouseDrag(int dx, int dy) { return PASS; }

		bool hasInteractions;
		std::string name;
		int layer;
	};

	class Canvas {
	public:
		Canvas(int x, int y, int width, int height);

		void handleEvents();
		void draw();

		void addElement(const std::string& name, std::shared_ptr<gl::CanvasElement> sprite);

		template<typename ElementType, typename ...Args>
		std::shared_ptr<ElementType> addElement(const std::string& name, Args ...args) {
			std::shared_ptr<ElementType> sprite = std::make_shared<ElementType>(args...);
			addElement(name, sprite);
			return sprite;
		}

		std::vector<std::shared_ptr<gl::CanvasElement>> getIntersectingElements(int x, int y, bool interactiveOnly = false, bool global = false);

		glm::ivec2 position;
		glm::ivec2 size;
		int maxLayers;
		bool shouldHandleEvents;

	private:
		std::shared_ptr<CanvasElement> mDraggedElement;
		std::vector<std::shared_ptr<gl::CanvasElement>> mElements;
	};

}