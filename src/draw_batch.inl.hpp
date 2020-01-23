#pragma once

#ifdef __INTELLISENSE__
#include "draw_batch.hpp"
#include "buffers.hpp"
#endif

#include "gl_internal.hpp"

namespace gl {

	namespace impl {
		template<typename T, typename... Args>
		void SetUniforms(Shader& shader, const std::string& location, T const& value, Args const&... rest) {
			shader.setUniform(location, value);
			if constexpr (sizeof...(rest) > 0) {
				SetUniforms(shader, rest...);
			}
		}
	}

	template<typename T, int d>
	std::shared_ptr<VertexBufferObject<T, d>> DrawBatch::addVertexAttribute(GLuint index) {
		assert(mVAO != 0);


		std::shared_ptr<VertexBufferObject<T, d>> vbo = std::make_shared<VertexBufferObject<T, d>>();
		vbo->target() = GL_ARRAY_BUFFER;
		vbo->usage() = GL_DYNAMIC_DRAW;
		vbo->update();

		addVertexAttribute(index, vbo);
		
		return vbo;
	}

	template<typename ...Args>
	std::shared_ptr<CompactVertexBufferObject<Args...>> DrawBatch::addVertexAttributes(GLuint initialIndex) {
		assert(mVAO != 0);

		typedef typename CompactVertexBufferObject<Args...> Buffer;

		std::shared_ptr<Buffer> vbo = std::make_shared<Buffer>();
		addVertexAttributes(initialIndex, vbo);
		
		return vbo;
	}

	template<typename T, int d>
	inline void DrawBatch::addVertexAttribute(GLuint index, std::shared_ptr<VertexBufferObject<T, d>> vbo)
	{
		assert(mVAO != 0);

		glBindVertexArray(mVAO);
		vbo->bind();
		mBufferIds.push_back(vbo->id());

		if constexpr (std::is_floating_point_v<T>) {
			glVertexAttribPointer(index, d, impl::to_glenum_v<T>, GL_FALSE, sizeof(T) * d, nullptr);
		}
		else {
			glVertexAttribIPointer(index, d, impl::to_glenum_v<T>, sizeof(T) * d, nullptr);
		}
		glEnableVertexAttribArray(index);

		vbo->unbind();
		glBindVertexArray(0);
		mVertexAttributes.push_back(vbo);
	}

	template<int entry, typename ...Args>
	inline void DrawBatch::addVertexAttribute(GLuint index, std::shared_ptr<CompactVertexBufferObject<Args...>>) {
		assert(mVAO != 0);

		glBindVertexArray(mVAO);
		vbo->bind();
		mBufferIds.push_back(vbo->id());

		if constexpr (std::is_floating_point_v<T>) {
			glVertexAttribPointer(index, d, impl::to_glenum_v<T>, GL_FALSE, sizeof(T) * d, nullptr);
		}
		else {
			glVertexAttribIPointer(index, d, impl::to_glenum_v<T>, sizeof(T) * d, nullptr);
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

		glBindVertexArray(mVAO);
		buffer->bind();
		impl::AddVertexAttribute<0, Buffer::value_type>(false, static_cast<int>(initialIndex));

		buffer->unbind();
		glBindVertexArray(0);
		mVertexAttributes.push_back(buffer);
	}

	template<typename T>
	void DrawBatch::setUniform(const std::string& location, T value) {
		mUniforms.push_back(std::make_shared<NamedUniform<T>>(location, value));
	}

	template<typename... Args>
	void DrawBatch::execute(const Args&... uniforms) {
		static_assert(sizeof...(Args) % 2 == 0, "Invalid number of arguments");
		assert(indexBuffer != nullptr);

		for (auto vbo : mVertexAttributes) {
			vbo->update();
		}
		if (indexBuffer != nullptr) {
			indexBuffer->update();
		}

		auto _ = shader.use();
		impl::SetUniforms(shader, uniforms...);
		for (auto uniform : mUniforms) {
			uniform->bind(shader);
		}

		glBindVertexArray(mVAO);
		if (primitiveType == GL_PATCHES) {
			glPatchParameteri(GL_PATCH_VERTICES, patchsize);
		}
		glDrawElements(primitiveType, indexBuffer->size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mIndexOffset * sizeof(GLuint)));

		glBindVertexArray(0);
		glUseProgram(0);
	}
}

