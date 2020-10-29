#pragma once

#include <memory>
#include <vector>

#include "buffers.hpp"
#include "shadermanager.hpp"

namespace gl {

	struct Uniform {
		virtual void bind(std::shared_ptr<Shader> shader) const  = 0;
		virtual void bind(Shader& shader) const  = 0;
	};

	template<typename T>
	struct NamedUniform : public Uniform {
		NamedUniform(const std::string& location, T value) :
			location(location),
			value(value) { }

		std::string location;
		T value;
		virtual void bind(std::shared_ptr<Shader> shader) const override {
			if constexpr (std::is_pointer_v<T>) {
				shader->setUniform(location, *value);
			}
			else {
				shader->setUniform(location, value);
			}
		}

		virtual void bind(gl::Shader& shader) const override {
			if constexpr (std::is_pointer_v<T>) {
				shader.setUniform(location, *value);
			}
			else {
				shader.setUniform(location, value);
			}
		}
	};

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

#include "draw_batch.inl.hpp"