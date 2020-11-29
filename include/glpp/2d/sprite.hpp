#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <string>

namespace gl {

	class Texture;
	class Shader;
	class Canvas;

	std::shared_ptr<gl::Shader> defaultSpriteShader();
	std::shared_ptr<gl::Texture> defaultSpriteTexture();

	class Sprite {
	public:
		Sprite();
		Sprite(std::string image, bool flipY = false);
		virtual void render(int width, int height, int layers);

		void setTexture(std::shared_ptr<gl::Texture> texture);
		std::shared_ptr<gl::Texture> setTexture(std::string path, bool flipY = false);

		virtual bool overlaps(int x, int y) const;

		std::shared_ptr<gl::Texture> texture;
		glm::vec3 color;
		glm::vec2 position;
		glm::vec2 size;
		glm::vec2 minUV, maxUV;
		int layer;
		std::shared_ptr<gl::Shader> shader;
		std::string name;
	};

}