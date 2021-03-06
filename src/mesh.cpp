#include "glpp/mesh.hpp"
#include "glpp/renderer.hpp"

gl::Mesh::Mesh() :
	mShowInOutliner(true),
	visible(true),
	name("Mesh"),
	ModelMatrix(1)
{
}

void gl::Mesh::drawOutliner()
{
}

gl::IndexBuffer& gl::Mesh::getIndexBuffer()
{
	return *mBatch.indexBuffer;
}