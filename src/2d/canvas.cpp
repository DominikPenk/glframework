#include "..\..\include\glpp\2d\canvas.hpp"

#include <glad/glad.h>

#include "glpp/2d/sprite.hpp"

gl::Canvas::Canvas(int x, int y, int width, int height) :
	position(x, y),
	size(width, height),
	maxLayers(1024)
{
}

void gl::Canvas::draw()
{
	glViewport(position.x, position.y, size.x, size.y);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (auto sprite : mSprites) {
		sprite->render(size.x, size.y, maxLayers);
	}
}

void gl::Canvas::addSprite(const std::string& name, std::shared_ptr<gl::Sprite> sprite)
{
	mSprites.push_back(sprite);
	sprite->name = name;
}
