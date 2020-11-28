#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace gl {

	class Renderer;
	class Sprite;

	class Canvas {
	public:
		Canvas(int x, int y, int width, int height);

		void draw();

		void addSprite(const std::string& name, std::shared_ptr<gl::Sprite> sprite);

		template<typename SpriteType, typename ...Args>
		std::shared_ptr<SpriteType> addSprite(const std::string& name, Args ...args) {
			std::shared_ptr<SpriteType> sprite = std::make_shared<SpriteType>(args...);
			addSprite(name, sprite);
			return sprite;
		}

		glm::ivec2 position;
		glm::ivec2 size;
		int maxLayers;

	private:

		std::vector<std::shared_ptr<gl::Sprite>> mSprites;
	};

}