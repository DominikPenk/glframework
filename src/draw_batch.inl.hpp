#pragma once

#ifdef __INTELLISENSE__
#include "draw_batch.hpp"
#include "buffers.hpp"
#endif

#include "gl_internal.hpp"

namespace gl {

	namespace impl {
		template<typename T, typename... Args>
		void SetUniforms(std::shared_ptr<Shader> shader, const std::string& location, T const& value, Args const&... rest) {
			shader->setUniform(location, value);
			if constexpr (sizeof...(rest) > 0) {
				SetUniforms(shader, rest...);
			}
		}

		template<typename T, typename... Args>
		void SetUniforms(gl::Shader& shader, const std::string& location, T const& value, Args const&... rest) {
			shader.setUniform(location, value);
			if constexpr (sizeof...(rest) > 0) {
				SetUniforms(shader, rest...);
			}
		}
	}

	template<typename T, int d>
	std::shared_ptr<VertexBufferObject<T, d>> DrawBatch::addVertexAttribute(GLuint index) {
		assert(VAO != 0);
		std::shared_ptr<VertexBufferObject<T, d>> vbo = std::make_shared<VertexBufferObject<T, d>>();
		vbo->target() = GL_ARRAY_BUFFER;
		vbo->usage() = GL_DYNAMIC_DRAW;
		vbo->update();
		addVertexAttribute(index, vbo);
		return vbo;
	}

	template<typename ...Args>
	std::shared_ptr<CompactVertexBufferObject<Args...>> DrawBatch::addVertexAttributes(GLuint initialIndex) {
		assert(VAO != 0);

		typedef typename CompactVertexBufferObject<Args...> Buffer;

		std::shared_ptr<Buffer> vbo = std::make_shared<Buffer>();
		addVertexAttributes(initialIndex, vbo);
		
		return vbo;
	}

	template<typename T>
	inline std::shared_ptr<VertexBufferObjectMap<T>> gl::DrawBatch::addVertexAttribute(GLuint index, T* data, size_t n)
	{
		assert(VAO != 0);

		std::shared_ptr<VertexBufferObjectMap<T>> vbo = std::make_shared<VertexBufferObjectMap<T>>(data, n);
		vbo->target() = GL_ARRAY_BUFFER;
		vbo->update();

		addVertexAttribute(index, vbo);

		return vbo;
	}

	template<typename T, int d>
	inline void DrawBatch::addVertexAttribute(GLuint index, std::shared_ptr<VertexBufferObject<T, d>> vbo)
	{
		assert(VAO != 0);

		glBindVertexArray(VAO);
		vbo->bind();
		mBufferIds.push_back(vbo->id());

		glEnableVertexAttribArray(index);
		if constexpr (std::is_floating_point_v<T>) {
			glVertexAttribPointer(index, d, impl::to_glenum_v<T>, GL_FALSE, sizeof(T) * d, nullptr);
		}
		else {
			glVertexAttribIPointer(index, d, impl::to_glenum_v<T>, sizeof(T) * d, nullptr);
		}

		vbo->unbind();
		glBindVertexArray(0);
		mVertexAttributes.push_back(vbo);
	}

	template<typename T>
	inline void DrawBatch::addVertexAttribute(GLuint index, std::shared_ptr<VertexBufferObjectMap<T>> vbo)
	{
		assert(VAO != 0);

		glBindVertexArray(VAO);
		vbo->bind();
		mBufferIds.push_back(vbo->id());

		if constexpr (impl::is_floating_point_v<T>) {
			glVertexAttribPointer(index, impl::get_dimension_v<T>, impl::to_glenum_v<T>, GL_FALSE, sizeof(T), nullptr);
		}
		else {
			glVertexAttribIPointer(index, impl::get_dimension_v<T>, impl::to_glenum_v<T>, sizeof(T), nullptr);
		}
		glEnableVertexAttribArray(index);

		vbo->unbind();
		glBindVertexArray(0);
		mVertexAttributes.push_back(vbo);
	}

	template<typename ...Args>
	inline void DrawBatch::addVertexAttributes(GLuint initialIndex, std::shared_ptr<CompactVertexBufferObject<Args...>> buffer)
	{
		typedef typename CompactVertexBufferObject<Args...> Buffer;


		buffer->target() = GL_ARRAY_BUFFER;
		buffer->usage() = GL_DYNAMIC_DRAW;
		buffer->update();

		glBindVertexArray(VAO);
		buffer->bind();
		impl::AddVertexAttribute<0, Buffer::value_type>(false, static_cast<int>(initialIndex));

		buffer->unbind();
		glBindVertexArray(0);
		mVertexAttributes.push_back(buffer);
	}

	template<typename ...Args>
	inline void DrawBatch::execute(gl::Shader& shader, const Args& ...uniforms)
	{
		static_assert(sizeof...(Args) % 2 == 0, "Invalid number of arguments");
		assert(indexBuffer != nullptr);

		for (auto vbo : mVertexAttributes) {
			vbo->update();
		}
		if (indexBuffer != nullptr) {
			indexBuffer->update();
		}

		auto _ = shader.use();
		if constexpr (sizeof...(Args) > 0) {
			impl::SetUniforms(shader, uniforms...);
		}

		glBindVertexArray(VAO);
		if (primitiveType == GL_PATCHES) {
			glPatchParameteri(GL_PATCH_VERTICES, patchsize);
		}
		glDrawElements(primitiveType, indexBuffer->size(), indexType, reinterpret_cast<void*>(indexOffset * sizeof(GLuint)));

		glBindVertexArray(0);
		glUseProgram(0);
	}
}

