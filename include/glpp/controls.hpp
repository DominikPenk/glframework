#pragma once

#include <memory>

#include <glm/glm.hpp>

struct ImGuiIO;

namespace gl {
	class Camera;

	class Control {
	public:
		Control();
		virtual void update(std::shared_ptr<Camera> camera, bool force = false);
		virtual void reset(std::shared_ptr<Camera> camera) = 0;

		int keyFrontalView, keyTopView, keyRightView;
	};

	class OrbitControl : public Control {
	public:
		OrbitControl(std::shared_ptr<Camera> cam);

		virtual void update(std::shared_ptr<Camera> camera, bool force = false) override;
		
		virtual void reset(std::shared_ptr<Camera> camera) override;

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

	};

	class FlyingControl : public Control {
	public:
		FlyingControl(std::shared_ptr<Camera> cam = nullptr);

		virtual void update(std::shared_ptr<Camera> camera, bool force = false) override;
		virtual void reset(std::shared_ptr<Camera> camera) override;
	
		float cameraSpeed;
		float sensitivity;
		bool constrainPitch;

		int keyForward;
		int keyBackward;
		int keyLeft;
		int keyRight;
		int keyUp;
		int keyDown;

	private:
		bool initialized;
		glm::vec3 startPos;
		glm::vec3 startViewDir;
	};
}