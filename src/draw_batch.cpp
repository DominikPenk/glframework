#include "draw_batch.hpp"

gl::DrawBatch::DrawBatch() :
	mVertexAttributes(),
	indexBuffer(std::make_shared<IndexBuffer>()),
	VAO(),
	indexOffset(0),
	primitiveType(GL_TRIANGLES),
	indexType(GL_UNSIGNED_INT),
	patchsize(0)
{
	indexBuffer->target() = GL_ELEMENT_ARRAY_BUFFER;
	
	glBindVertexArray(VAO);
	indexBuffer->bind();
	glBindVertexArray(0);
	indexBuffer->unbind();
}

gl::DrawBatch::~DrawBatch()
{
}
