#include "..\..\include\glpp\2d\canvas.hpp"

#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glpp/2d/sprite.hpp"
#include "glpp/imgui.hpp"

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

	// Enable depth test
	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean blendEnabled = glIsEnabled(GL_BLEND);
	GLint blendSrc;
	GLint blendDst;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto sprite : mElements) {
		sprite->draw(size.x, size.y, maxLayers);
	}

	glBlendFunc(blendSrc, blendDst);
	if (!depthTestEnabled) {
		glDisable(GL_BLEND);
	}
	if (!blendEnabled) {
		glDisable(GL_BLEND);
	}
}

void gl::Canvas::addElement(const std::string& name, std::shared_ptr<gl::CanvasElement> element)
{
	mElements.push_back(element);
	element->name = name;
}

std::vector<std::shared_ptr<gl::CanvasElement>> gl::Canvas::getIntersectingElements(int x, int y, bool global)
{
	if (global) {
		ImGuiIO io = ImGui::GetIO();
		int w, h;
		glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
		x = io.MousePos.x - position.x;
		y = h - io.MousePos.y - 1 - position.y;
	}
	if (x < 0 || y < 0 || x >= size.x || y >= size.y)
		return {};

	std::vector<std::shared_ptr<gl::CanvasElement>> activeElements;
	for (auto element : mElements) 
	{
		if (element->overlaps(x, y) && element->blocksRaycasts) {
			activeElements.push_back(element);
		}
	}

	std::stable_sort(activeElements.begin(), activeElements.end(), [](auto s1, auto s2) {
		return s1->layer > s2->layer;
	});


	return activeElements;
}

gl::CanvasElement::CanvasElement() :
	layer(0),
	blocksRaycasts(true),
	name("Canvas Element")
{
}
