#pragma once

#include <memory>

#include <glm/glm.hpp>

struct ImGuiIO;

namespace gl {
	class Camera;

	class Control {
	public:
		virtual void update(std::shared_ptr<Camera> camera) = 0;
		virtual void reset(std::shared_ptr<Camera> camera) = 0;
	};

	class OrbitControl : public Control {
	public:
		OrbitControl(std::shared_ptr<Camera> cam);

		// Inherited via Control
		virtual void update(std::shared_ptr<Camera> camera) override;

		glm::vec3 target;
		glm::vec3 orbital_axis;
		float minDistance, maxDistance;
		float minZoom, maxZoom;
		float minPolarAngle, maxPolarAngle;
		float minAzimuthAngle, maxAzimuthAngle;
		bool enableDamping;
		float dampingFactor;
		
		bool enableZoom;
		float zoomSpeed;

		bool enableRotate;
		float rotateSpeed;

		bool enablePan;
		float panSpeed;
		float keyPanSpeed;

		bool autoRotate;
		float autoRotateSpeed;

		bool enableKeys;

	private:

		glm::vec3 _target0;
		glm::vec3 _position0;
		glm::vec3 _sphericalDelta;
		glm::vec3 _panOffset;

		// Inherited via Control
		virtual void reset(std::shared_ptr<Camera> camera) override;
	};

}