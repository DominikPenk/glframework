#include "glpp/controls.hpp"

#include <algorithm>

#include <glm/gtx/matrix_cross_product.hpp>

#include "glpp/imgui.hpp"

#include "glpp/camera.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

glm::mat3 rotationFromUnitVectors(const glm::vec3& a, const glm::vec3& b) {
	glm::vec3 v = glm::cross(a, b);
	glm::mat3 V = glm::matrixCross3(v);
	float c = glm::dot(a, b);
	return glm::mat3(1.0f) + V + 1.0f / (1.0f + c) * V * V;
}

glm::vec3 pan(float deltaX, float deltaY, std::shared_ptr<gl::Camera>& cam, glm::vec3 target, bool screenSpacePanning)
{
	glm::vec3 position = cam->position();
	glm::vec3 offset = position - target;
	float distance = (float)offset.length();

	distance *= std::tan(glm::radians(cam->fov / 2));

	float aspect = cam->ScreenWidth / cam->ScreenHeight;

	glm::vec3 panOffset = 2.0f * deltaX * distance/ cam->ScreenHeight * aspect * glm::vec3(cam->viewMatrix[0]);
	float d = 2 * deltaY * distance / cam->ScreenHeight;
	if (screenSpacePanning) {
		panOffset -= d * glm::vec3(cam->viewMatrix[1]);
	}
	else {
		panOffset -= d * glm::vec3(cam->viewMatrix[3]);
	}

	return panOffset;
}

gl::OrbitControl::OrbitControl(std::shared_ptr<gl::Camera> cam)
{
	target = glm::vec3(0, 0, 0);
	
	minDistance = 0.0f;
	maxDistance = std::numeric_limits<float>::infinity();

	constexpr float Infinity = std::numeric_limits<float>::infinity();

	// How far you can zoom in and out ( OrthographicCamera only )
	minZoom = 0.0f;
	maxZoom = Infinity;

	// How far you can orbit vertically, upper and lower limits.
	// Range is 0 to Math.PI radians.
	minPolarAngle = 0.0f; // radians
	maxPolarAngle = 3.14159265359f; // radians

	// How far you can orbit horizontally, upper and lower limits.
	// If set, must be a sub-interval of the interval [ - Math.PI, Math.PI ].
	minAzimuthAngle = -Infinity; // radians
	maxAzimuthAngle = Infinity; // radians

	// Set to true to enable damping (inertia)
	// If damping is enabled, you must call controls.update() in your animation loop
	enableDamping = false;
	dampingFactor = 0.05;

	// This option actually enables dollying in and out; left as "zoom" for backwards compatibility.
	// Set to false to disable zooming
	enableZoom = true;
	zoomSpeed = 1.0f;

	// Set to false to disable rotating
	enableRotate = true;
	rotateSpeed = 1.0f;

	// Set to false to disable panning
	enablePan = true;
	panSpeed = 3.0f;
	keyPanSpeed = 2.0f;	// pixels moved per arrow key push

	// Set to true to automatically rotate around the target
	// If auto-rotate is enabled, you must call controls.update() in your animation loop
	autoRotate = false;
	autoRotateSpeed = 2.0f; // 30 seconds per round when fps is 60

	// Set to false to disable use of the keys
	enableKeys = true;

	orbital_axis = glm::vec3(0, 1, 0);

	// for reset
	_target0 = target;
	_position0 = cam->position();
	_sphericalDelta = glm::vec3(0, 0, 1);
	_panOffset = glm::vec3(0, 0, 0);
}

void gl::OrbitControl::update(std::shared_ptr<Camera> camera)
{
	ImGuiIO io = ImGui::GetIO();

	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
		bool hadUserInteraction = false;

		// Handle keyboard input
		glm::vec3 panOffset = glm::vec3(0);
		if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
			panOffset = pan(0, keyPanSpeed, camera, target, false);
			hadUserInteraction = true;
		}
		else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			panOffset = pan(0, -keyPanSpeed, camera, target, false);
			hadUserInteraction = true;
		}
		else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
			panOffset = pan(-keyPanSpeed, 0, camera, target, false);
			hadUserInteraction = true;
		}
		else if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
		{
			panOffset = pan(keyPanSpeed, 0, camera, target, false);
			hadUserInteraction = true;
		}
		
		

		// Handle mouse input
		float scale = 1.0f;
		if (io.MouseDown[0] && !io.KeyCtrl) {
			glm::vec2 rotateDelta = glm::vec2(io.MouseDelta[0], io.MouseDelta[1]) * rotateSpeed;

			_sphericalDelta.x -= glm::two_pi<float>() * rotateDelta.y / camera->ScreenHeight;
			_sphericalDelta.y -= glm::two_pi<float>() * rotateDelta.x / camera->ScreenHeight;
			hadUserInteraction = true;
		}
		else if (io.MouseDown[1]) {
			panOffset = pan(-io.MouseDelta.x * panSpeed, io.MouseDelta.y * panSpeed, camera, target, true);
			hadUserInteraction = true;
		}
		else if (io.MouseWheel > 0) {
			scale *= std::pow(0.95, zoomSpeed);
			hadUserInteraction = true;
		}
		else if (io.MouseWheel < 0) {
			scale /= std::pow(0.95, zoomSpeed);
			hadUserInteraction = true;
		}

		_panOffset += panOffset;

		// Apply auto rotation
		if (autoRotate && !hadUserInteraction) {
			_sphericalDelta.y -= glm::radians(autoRotateSpeed);
		}

		// Update if required
		if (hadUserInteraction || autoRotate) {
			glm::mat3 rot = rotationFromUnitVectors(orbital_axis, glm::vec3(0, 1, 0));

			glm::vec3 position = camera->position();
			glm::vec3 offset = position - target;

			offset = rot * offset;

			// Compute spherical coordinates
			float radius = std::sqrt(glm::dot(offset, offset));
			float theta = std::atan2(offset.x, offset.z);
			float phi = std::acos(std::max(std::min(offset.y / radius, 1.0f), -1.0f));

			phi += enableDamping ? _sphericalDelta.x * dampingFactor : _sphericalDelta.x;
			theta += enableDamping ? _sphericalDelta.y * dampingFactor : _sphericalDelta.y;

			phi = std::max(minPolarAngle, std::min(maxPolarAngle, phi));
			theta = std::max(minAzimuthAngle, std::min(maxAzimuthAngle, theta));
			radius = std::max(minDistance, std::min(maxDistance, radius * scale));

			// Restrict phi to [0.000001, pi - 0.00001]
			phi = std::max(0.000001f, std::min(glm::pi<float>() - 0.000001f, phi));

			// Convert back to eucledian
			float sinPhiRadius = std::sin(phi) * radius;
			offset.x = sinPhiRadius * std::sin(theta);
			offset.y = std::cos(phi) * radius;
			offset.z = sinPhiRadius * std::cos(theta);

			offset = glm::transpose(rot) * offset;
			_panOffset = enableDamping ? dampingFactor * _panOffset : _panOffset;
			target += _panOffset;

			camera->lookAt(target + _panOffset, target + offset);

			if (enableDamping) {
				_sphericalDelta.y *= (1.0f - dampingFactor);
				_sphericalDelta.x *= (1.0f - dampingFactor);
				_panOffset *= (1.0f - dampingFactor);
			}
			else {
				_sphericalDelta = glm::vec3(0, 0, 1);
				_panOffset = glm::vec3(0, 0, 0);
			}
		}


	}


	camera->lookAt(target);
}

void gl::OrbitControl::reset(std::shared_ptr<Camera> camera)
{
	//camera->Position = _position0;
}

gl::FlyingControl::FlyingControl(std::shared_ptr<Camera> cam) :
	keyForward(GLFW_KEY_W),
	keyBackward(GLFW_KEY_S),
	keyLeft(GLFW_KEY_A),
	keyRight(GLFW_KEY_D),
	keyUp(GLFW_KEY_Q),
	keyDown(GLFW_KEY_E),
	cameraSpeed(1.f),
	sensitivity(0.1f),
	constrainPitch(true)
{
	if (cam != nullptr) {
		initialized = true;
		startPos = cam->position();
		startViewDir = cam->forward();
	}
}

void gl::FlyingControl::update(std::shared_ptr<Camera> camera)
{
	const glm::vec3 Front = camera->forward();
	const glm::vec3 Right = glm::normalize(glm::cross(Front, camera->up()));
	const glm::vec3 Up = camera->up();

	ImGuiIO io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
		bool hadUserInteraction = false;

		const float deltaTime = io.DeltaTime;

		// Keyboard
		float velocity = cameraSpeed * deltaTime;
		glm::vec3 position = camera->position();
		if (io.KeysDown[keyForward]) {
			position += Front * velocity;
		}
		if (io.KeysDown[keyBackward]) {
			position -= Front * velocity;
		}
		if (io.KeysDown[keyLeft]) {
			position -= Right * velocity;
		}
		if (io.KeysDown[keyRight]) {
			position += Right * velocity;
		}
		if (io.KeysDown[keyUp]) {
			position += Up * velocity;
		}
		if (io.KeysDown[keyDown]) {
			position -= Up * velocity;
		}

		// Mouse movement
		float pitch = std::asin(Front.y);
		float yaw = std::atan2(Front.z, Front.x);

		if (ImGui::IsMouseDown(1)) {
			yaw += io.MouseDelta.x * glm::radians(sensitivity);
			pitch += io.MouseDelta.y * glm::radians(sensitivity);

			if (constrainPitch) {
				pitch = std::clamp(pitch, -glm::radians(89.0f), glm::radians(89.0f));
			}
		}

		glm::vec3 front;
		front.x = std::cos(yaw) * std::cos(pitch);
        front.y = std::sin(pitch);
        front.z = std::sin(yaw) * std::cos(pitch);
        
		camera->lookAt(position + front, position);
	
		// Scroll wheel
		if (io.MouseWheel > 0) {
			cameraSpeed /= std::pow(0.95f, cameraSpeed);
		}
		else if (io.MouseWheel < 0) {
			cameraSpeed *= std::pow(0.95f, cameraSpeed);
			hadUserInteraction = true;
		}
	}
}

void gl::FlyingControl::reset(std::shared_ptr<Camera> camera)
{
}
