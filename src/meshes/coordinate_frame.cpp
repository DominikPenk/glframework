#include "glpp/meshes/coordinate_frame.hpp"
#include "glpp/renderer.hpp"

#include "../shaders/axis.glsl.h"

gl::CoordinateFrame::CoordinateFrame(float length) :
	Mesh(),
	axisLength(length)
{
	mPoints.push_back(glm::vec3(0));
	mPoints.target() = GL_ARRAY_BUFFER;
	mPoints.usage() = GL_DYNAMIC_DRAW;

	mVAO.addVertexAttribute(mPoints, 0);

	//mShader = std::string(GL_FRAMEWORK_SHADER_DIR) + "axis.glsl";
	mShader = Shader(AXIS_SHADER);

	mShowInOutliner = false;
}

void gl::CoordinateFrame::render(const std::shared_ptr<gl::Camera> camera)
{
	mPoints.update();

	auto _ = mShader.use();

	glm::mat4 P = camera->GetProjectionMatrix();
	glm::mat4 V = camera->viewMatrix;
	glm::mat4 MVP = P * V * ModelMatrix;
	mShader.setUniform("MVP", MVP);
	mShader.setUniform("length", axisLength);

	mVAO.bind();
	glDrawArrays(GL_POINTS, 0, 1);
	mVAO.unbind();

	glUseProgram(0);
}