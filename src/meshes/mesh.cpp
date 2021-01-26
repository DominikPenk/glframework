#include "glpp/meshes/mesh.hpp"
#include "glpp/renderer.hpp"
#include <glpp/camera.hpp>

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