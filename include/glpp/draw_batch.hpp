#pragma once

#include <memory>
#include <vector>

#include "glpp/buffers.hpp"
#include "glpp/shadermanager.hpp"

namespace gl {

	class DrawBatch {
	public:
		DrawBatch();
		~DrawBatch();

		template<typename... Args>
		void execute(gl::Shader& shader, const Args&... uniforms);

		template<typename T, int d>
		std::shared_ptr<VertexBufferObject<T, d>> addVertexAttribute(GLuint index);

		template<typename ...Args>
		std::shared_ptr<CompactVertexBufferObject<Args...>> addVertexAttributes(GLuint initialIndex = 0);

		template<typename T>
		std::shared_ptr<VertexBufferObjectMap<T>> addVertexAttribute(GLuint index, T* data, size_t n);

		template<typename T, int d>
		void addVertexAttribute(GLuint index, std::shared_ptr<VertexBufferObject<T, d>> buffer);

		template<typename T>
		void addVertexAttribute(GLuint index, std::shared_ptr<VertexBufferObjectMap<T>> buffer);

		template<typename ...Args>
		void addVertexAttributes(GLuint initialIndex, std::shared_ptr<CompactVertexBufferObject<Args...>> buffer);
		
		template<typename Buffertype = VertexBufferObjectBase>
		std::shared_ptr<Buffertype> getAttirbute(int index);

		void clear() {
			for (auto attribute : mVertexAttributes) {
				attribute->clear();
			}
			indexBuffer->clear();
		}


		std::shared_ptr<IndexBuffer> indexBuffer;
		GLenum primitiveType;
		GLenum indexType;
		unsigned int patchsize;

		unsigned int indexOffset;
		VAOIndex VAO;

	private:
		std::vector<std::shared_ptr<VertexBufferObjectBase>> mVertexAttributes;
	};
}

#include "glpp/draw_batch.inl.hpp"