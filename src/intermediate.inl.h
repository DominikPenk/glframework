#pragma once

#include "gl_internal.hpp"

#include "buffers.hpp"
#include "intermediate.h"

template<typename ...Uniforms>
void gl::fullscreenTriangle(int x, int y, int width, int height, gl::Shader& shader, Uniforms... uniforms)
{
	static_assert(sizeof...(Uniforms) % 2 == 0, "Invalid number of arguments");

	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean depthWriteEnabled;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteEnabled);

	auto _ = shader.use();
	if constexpr (sizeof...(Uniforms) > 0) {
		impl::SetUniforms(shader, uniforms...);
	}
	glViewport(x, y, width, height);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	gl::VertexArrayObject::bindDummy();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	if (depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
	glDepthMask(depthWriteEnabled);
}

template<typename ...Uniforms>
void gl::fullscreenTriangle(int x, int y, int width, int height, std::shared_ptr<gl::Shader> shader, Uniforms ...uniforms)
{
	fullscreenTriangle(x, y, width, height, *shader, uniforms...);
}
