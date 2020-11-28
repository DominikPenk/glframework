#pragma once

#include "glpp/gl_internal.hpp"

template<typename ...Uniforms>
inline void gl::Shader::setUniforms(Uniforms ...uniforms)
{
	static_assert(sizeof...(Uniforms) % 2 == 0, "Invalid number of arguments");
	if constexpr (sizeof...(Uniforms) > 0) {
		impl::SetUniforms(*this, uniforms...);
	}
}