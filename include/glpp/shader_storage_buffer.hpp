#pragma once

#include <glad/glad.h>

namespace gl {

	class ShaderStorageBuffer {
	public:
		ShaderStorageBuffer();
		~ShaderStorageBuffer();

		void update(const void const* data, size_t sizeInBytes);

		template<typename T> 
		void update(const T& data) {
			update((const void const*)&data, sizeof(T));
		}

		void download(void* dst, size_t sizeInBytes);
		template<typename T>
		void download(T& data) {
			download((void*)&data, sizeof(T));
		}

		void bind(int slot);
		void unbind();

	protected:
		
		GLuint mId;

	};

}