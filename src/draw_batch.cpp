#include "draw_batch.hpp"

gl::DrawBatch::DrawBatch() :
	mVertexAttributes(),
	indexBuffer(std::make_shared<IndexBuffer>()),
	mVAO(),
	mUniforms(),
	shader(),
	mIndexOffset(0),
	primitiveType(GL_TRIANGLES),
	patchsize(0)
{
	indexBuffer->target() = GL_ELEMENT_ARRAY_BUFFER;
	
	glBindVertexArray(mVAO);
	indexBuffer->bind();
	glBindVertexArray(0);
	indexBuffer->unbind();
}

gl::DrawBatch::~DrawBatch()
{
}

void gl::DrawBatch::execute()
{
	assert(indexBuffer != nullptr);

	for (auto vbo : mVertexAttributes) {
		vbo->update();
	}
	if (indexBuffer != nullptr) {
		indexBuffer->update();
	}

	auto _ = shader->use();
	for (auto uniform : mUniforms) {
		uniform->bind(shader);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(mVAO);
	
	if (primitiveType == GL_PATCHES) {
		glPatchParameteri(GL_PATCH_VERTICES, patchsize);
	}
	glDrawElements(primitiveType, indexBuffer->size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mIndexOffset * sizeof(GLuint)));
	
	glBindVertexArray(0);
	glUseProgram(0);
}