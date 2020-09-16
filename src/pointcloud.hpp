#pragma once

#include "buffers.hpp"
#include "mesh.hpp"


namespace gl {

	class PointCloud : public gl::Mesh {
	public:
		PointCloud();
		PointCloud(const std::vector<glm::vec3>& points, glm::vec3 color);
		PointCloud(const std::vector<glm::vec3>& points, std::vector<glm::vec3> color);
		PointCloud(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points);

		const glm::vec3& color(int i) const;
		glm::vec3& color(int i);

		const glm::vec3& position(std::size_t i) const;
		glm::vec3& position(std::size_t i);

		const std::tuple<glm::vec3, glm::vec3>& point(int i) const;
		std::tuple<glm::vec3, glm::vec3>& point(int i);

		void addPoint(const glm::vec3& position, const glm::vec3& color);
		void addPoints(const std::vector<glm::vec3> points, const glm::vec3& color);
		void addPoints(const std::vector<glm::vec3> points, const std::vector<glm::vec3>& color);
		void addPoints(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points);

		void setPoints(const std::vector<std::tuple<glm::vec3, glm::vec3>>& points);

		void clear();

		std::size_t size() const;

		virtual void render(const RendererBase* env) override;
		virtual void drawOutliner() override;

		int pointSize;

	protected:
		std::shared_ptr<CompactVertexBufferObject<glm::vec3, glm::vec3>> data;
		DrawBatch mBatch;
	};
}