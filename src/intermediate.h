#pragma once

#include <shadermanager.hpp>

namespace gl {

	std::shared_ptr<gl::Shader> textureDispShader(bool uvUp = true);

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, gl::Shader& shader, Uniforms... uniforms);

	template<typename... Uniforms>
	void fullscreenTriangle(int x, int y, int width, int height, std::shared_ptr<gl::Shader> shader, Uniforms... uniforms);

	void displayTexture(int x, int y, int width, int height, std::shared_ptr<gl::Texture> tex, bool uvUp = true);
	void displayTexture(int x, int y, int width, int height, gl::Texture& tex, bool uvUp = true);

}

#include "intermediate.inl.h"