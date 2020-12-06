#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace gl {

	class Renderer;
	class Sprite;
	class EventSystem;

	class CanvasElement {
	public:
		CanvasElement();
		virtual bool overlaps(float x, float y) const = 0;
		virtual void draw(int x, int y, int layers) = 0;
		
		bool blocksRaycasts;
		std::string name;
		int layer;
	};

	class Canvas {
	public:
		Canvas(int x, int y, int width, int height);

		void draw();

		void addElement(const std::string& name, std::shared_ptr<gl::CanvasElement> sprite);

		template<typename ElementType, typename ...Args>
		std::shared_ptr<ElementType> addElement(const std::string& name, Args ...args) {
			std::shared_ptr<ElementType> sprite = std::make_shared<ElementType>(args...);
			addElement(name, sprite);
			return sprite;
		}

		std::vector<std::shared_ptr<gl::CanvasElement>> getIntersectingElements(int x, int y, bool global = false);

		glm::ivec2 position;
		glm::ivec2 size;
		int maxLayers;

	private:
		std::vector<std::shared_ptr<gl::CanvasElement>> mElements;
	};

}