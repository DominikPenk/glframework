#include "glpp/shader_storage_buffer.hpp"

#include <cstring>

gl::ShaderStorageBuffer::ShaderStorageBuffer()
{
	glGenBuffers(1, &mId);
}

gl::ShaderStorageBuffer::~ShaderStorageBuffer()
{
	glDeleteBuffers(1, &mId);
}

void gl::ShaderStorageBuffer::update(const void const* data, size_t sizeInBytes)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeInBytes, data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void gl::ShaderStorageBuffer::download(void* dst, size_t sizeInBytes)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mId);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	std::memcpy(dst, p, sizeInBytes);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void gl::ShaderStorageBuffer::bind(int slot)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, mId);
}

void gl::ShaderStorageBuffer::unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
