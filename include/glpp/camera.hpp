#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <iostream>

#undef near
#undef far

#define CAMERA_MOVEMENT_SPEED 5.0f

namespace gl {
	class Camera
	{
	public:
		// Camera Attributes
		glm::mat4 viewMatrix;
		float fov;
		int ScreenHeight, ScreenWidth;
		float Near, Far;

		// Constructor with vectors
		Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float fov = 50.0f) :
			
			fov(fov),
			Near(0.01f),
			Far(-5.0f),
			ScreenWidth(1),
			ScreenHeight(1)
		{
			viewMatrix = glm::lookAt(position, target, up);
		}

		void lookAt(glm::vec3 target) {
			viewMatrix = glm::lookAt(position(), target, glm::vec3(0, 1, 0));
		}

		void lookAt(glm::vec3 target, glm::vec3 position, glm::vec3 up = glm::vec3(0, 1, 0)) {
			viewMatrix = glm::lookAt(position, target, up);
		}

		// Returns the projection matrix
		glm::mat4 GetProjectionMatrix(float near = -1.f, float far = -1.0f) const {
			return glm::perspective(glm::radians(fov), (float)ScreenWidth / (float)ScreenHeight,
				near > 0 ? near : Near,
				far > 0 ? far : Far);
		}

		// Returns the projection matrix
		glm::mat4 GetOrthographicMatrix(float near = -1.f, float far = -1.0f) const {
			return glm::ortho(-1.f, 1.f, 1.f, -1.f,
				near > 0 ? near : Near,
				far > 0 ? far : Far);
		}

		glm::vec3 position() const {
			return glm::inverse(viewMatrix) * glm::vec4(0, 0, 0, 1);
		}

		glm::vec3 up() const {
			return glm::inverse(viewMatrix) * glm::vec4(0, 1, 0, 0);
		}

		glm::vec3 forward() const {
			return glm::inverse(viewMatrix) * glm::vec4(0, 0, -1, 0);
		}

		void translate(const glm::vec3& t) {
			viewMatrix =  glm::translate(viewMatrix, t);
		}

		/// <summary>
		/// Create a ray trough pixel (x, y) in view space
		/// </summary>
		/// <remark>Top left pixel is (0, 0)</remark>
		/// <param name="x">Pixel x-coordinate</param>
		/// <param name="y">Pixel y-coordinate</param>
		/// <returns>The direction of the ray trough pixel (x, y)</returns>
		glm::vec4 viewSpaceRay(float x, float y) {
			// Convert to normalized device coordinates
			float ndcX = 2.0f * x / ScreenWidth - 1.0f;
			float ndcY = -(2.0f * y / ScreenHeight - 1.0f);
			glm::vec4 clip(ndcX, ndcY, -1.0f, 1.0f);
			glm::vec4 eye = glm::inverse(GetProjectionMatrix()) * clip;
			eye.z = -1.f;
			eye.w = 0.f;
			return glm::normalize(eye);
		}

		glm::vec3 viewSpaceRay3(float x, float y) {
			// Convert to normalized device coordinates
			float ndcX = 2.0f * x / ScreenWidth - 1.0f;
			float ndcY = -(2.0f * y / ScreenHeight - 1.0f);
			glm::vec4 clip(ndcX, ndcY, -1.0f, 1.0f);
			glm::vec4 eye = glm::inverse(GetProjectionMatrix()) * clip;
			eye.z = -1.f;
			eye.w = 0.f;
			eye = glm::normalize(eye);
			return glm::vec3(eye);
		}

		/// <summary>
		/// Create a ray trough pixel (x, y) in world space
		/// </summary>
		/// <remark>Top left pixel is (0, 0)</remark>
		/// <param name="x">Pixel x-coordinate</param>
		/// <param name="y">Pixel y-coordinate</param>
		/// <returns>A pair of vectors, the first is the ray origin and the second the view direction</returns>
		std::pair<glm::vec4, glm::vec4> worldSpaceRay(float x, float y) {
			// Convert to normalized device coordinates
			float ndcX = 2.0f * x / ScreenWidth - 1.0f;
			float ndcY = -(2.0f * y / ScreenHeight - 1.0f);
			glm::vec4 clip(ndcX, ndcY, -1.0f, 1.0f);
			glm::vec4 eye = glm::inverse(GetProjectionMatrix()) * clip;
			eye.z = -1.f;
			eye.w = 0.f;
			glm::mat4 invV = glm::inverse(viewMatrix);
			return std::make_pair(
				invV * glm::vec4(0, 0, 0, 1),
				invV * eye
			);
		}
		
	};

}