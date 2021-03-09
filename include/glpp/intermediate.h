#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include "glpp/shadermanager.hpp"

namespace gl {
	class Texture;
	class Camera;

	std::shared_ptr<gl::Shader> textureDispShader();
	std::shared_ptr<gl::Shader> scalarDispShader();

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, gl::Shader& shader, Uniforms... uniforms);

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, std::shared_ptr<gl::Shader> shader, Uniforms... uniforms);


	void displayTexture(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, glm::vec2 uvmin = glm::vec2(0), glm::vec2 umax = glm::vec2(1));
	void displayTexture(int x, int y, int width, int height, gl::Texture& tex, glm::vec2 uvmin = glm::vec2(0), glm::vec2 uvmax = glm::vec2(1));

	void displayScalars(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, glm::vec2 range, glm::vec4 minColor=glm::vec4(1), glm::vec4 maxColor=glm::vec4(1, 0, 0, 1), glm::vec2 uvmin = glm::vec2(0), glm::vec2 uvmax = glm::vec2(1));
	void displayScalars(int x, int y, int width, int height, gl::Texture& tex, glm::vec2 range, glm::vec4 minColor = glm::vec4(1), glm::vec4 maxColor = glm::vec4(1, 0, 0, 1), glm::vec2 uvmin = glm::vec2(0), glm::vec2 uvmax = glm::vec2(1));

}

#include "glpp/intermediate.inl.h"