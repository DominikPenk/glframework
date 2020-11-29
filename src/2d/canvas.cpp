#include "..\..\include\glpp\2d\canvas.hpp"

#include <algorithm>

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

std::vector<std::shared_ptr<gl::Sprite>> gl::Canvas::getIntersectingSprites(int global_x, int global_y)
{
	const int lx = global_x - position.x;
	const int ly = global_y - position.y;
	if (lx < 0 || ly < 0 || lx >= size.x || ly >= size.y)
		return {};

	std::vector<std::shared_ptr<gl::Sprite>> activeSprites;
	for (auto sprite : mSprites) 
	{
		if (sprite->overlaps(lx, ly)) {
			activeSprites.push_back(sprite);
		}
	}

	std::stable_sort(activeSprites.begin(), activeSprites.end(), [](auto s1, auto s2) {
		return s1->layer > s2->layer;
	});


	return activeSprites;
}
