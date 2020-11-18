#pragma once

#include <shadermanager.hpp>

namespace gl {

	std::shared_ptr<gl::Shader> textureDispShader();

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, gl::Shader& shader, Uniforms... uniforms);

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, std::shared_ptr<gl::Shader> shader, Uniforms... uniforms);


	void displayTexture(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, glm::vec2 uvmin = glm::vec2(0), glm::vec2 umax = glm::vec2(1));
	void displayTexture(int x, int y, int width, int height, gl::Texture& tex, glm::vec2 uvmin = glm::vec2(0), glm::vec2 uvmax = glm::vec2(1));

}

#include "intermediate.inl.h"