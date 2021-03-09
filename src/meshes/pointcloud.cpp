#include "glpp/meshes/pointcloud.hpp"

#include "glpp/renderer.hpp"

#include <numeric>

gl::PointCloud::PointCloud() :
	Mesh(),
	data(nullptr),
	pointSize(10)
{
	data = mBatch.addVertexAttributes<glm::vec3, glm::vec3>();
	mBatch.primitiveType = GL_POINTS;
	mShader = gl::Shader(std::string(GL_FRAMEWORK_SHADER_DIR) + "pointcloud.glsl");
}

gl::PointCloud::PointCloud(const std::vector<glm::vec3>& points, glm::vec3 color) :
	PointCloud()
{
	addPoints(points, color);
}

gl::PointCloud::PointCloud(const std::vector<glm::vec3>& points, std::vector<glm::vec3> color) :
	PointCloud()
{
	addPoints(points, color);
}

gl::PointCloud::PointCloud(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points) :
	PointCloud()
{
	addPoints(points);
}

const glm::vec3& gl::PointCloud::color(int i) const
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at<1>(i);
}

glm::vec3& gl::PointCloud::color(int i)
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at<1>(i);
}

const glm::vec3& gl::PointCloud::position(std::size_t i) const
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at<0>(i);
}

glm::vec3& gl::PointCloud::position(std::size_t i)
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at<0>(i);
}

const std::tuple<glm::vec3, glm::vec3>& gl::PointCloud::point(int i) const
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at(i);
}

std::tuple<glm::vec3, glm::vec3>& gl::PointCloud::point(int i)
{
	assert(data != nullptr);
	assert(i >= 0 && i < data->size());
	return data->at(i);
}

void gl::PointCloud::addPoint(const glm::vec3& position, const glm::vec3& color)
{
	data->push_back(position, color);
	mBatch.indexBuffer->push_back((unsigned int)mBatch.indexBuffer->size());
}

void gl::PointCloud::addPoints(const std::vector<glm::vec3> points, const glm::vec3& color)
{
	unsigned int idx0 = mBatch.indexBuffer->size();
	for (std::size_t i = 0; i < points.size(); ++i) {
		data->push_back(points[i], color);
		mBatch.indexBuffer->push_back((unsigned int)i + idx0);
	}
}

void gl::PointCloud::addPoints(const std::vector<glm::vec3> points, const std::vector<glm::vec3>& color)
{
	unsigned int idx0 = mBatch.indexBuffer->size();
	for (std::size_t i = 0; i < points.size(); ++i) {
		data->push_back(points[i], color[i]);
		mBatch.indexBuffer->push_back((unsigned int)i + idx0);
	}
}

void gl::PointCloud::addPoints(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points)
{
	unsigned int idx0 = mBatch.indexBuffer->size();
	data->resize(data->size() + points.size());
	for (std::size_t i = 0; i < points.size(); ++i) {
		data->at(i + idx0) = points[i];
		mBatch.indexBuffer->push_back((unsigned int)i + idx0);
	}
}

void gl::PointCloud::setPoints(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points)
{
	
	data->resize(points.size());
	mBatch.indexBuffer->clear();
	for (std::size_t i = 0; i < points.size(); ++i) {
		data->at(i) = points[i];
		mBatch.indexBuffer->push_back((unsigned int)i);
	}
}

void gl::PointCloud::setPoints(const std::vector<glm::vec3>& points, glm::vec3 color)
{
	data->resize(points.size());
	mBatch.indexBuffer->clear();
	for (std::size_t i = 0; i < points.size(); ++i) {
		data->at(i) = std::make_tuple(points[i], color);
		mBatch.indexBuffer->push_back((unsigned int)i);
	}
}

void gl::PointCloud::clear()
{
	mBatch.indexBuffer->clear();
	data->clear();
}

std::size_t gl::PointCloud::size() const
{
	return mBatch.indexBuffer->size();
}

void gl::PointCloud::render(const std::shared_ptr<gl::Camera> camera)
{
	glm::mat4 P = camera->GetProjectionMatrix();
	glm::mat4 V = camera->viewMatrix;
	glm::mat4 MV = V * ModelMatrix;
	mBatch.execute(
		mShader,
		"MV", MV,
		"P", P,
		"pointsize", static_cast<float>(pointSize) / camera->ScreenWidth);
}

void gl::PointCloud::drawOutliner()
{
	ImGui::DragInt("Point Size", &pointSize, 1.f, 3, 50);
}
