#pragma once

#include <glm/glm.hpp>

#include <glm/gtx/transform.hpp>

namespace ImGui3D {

	/// <summary>Compute a matrix converting points in the orthonormal space defined by n to world coordinates</summary>
	/// <remark>Implemented using https://graphics.pixar.com/library/OrthonormalB/paper.pdf </remark>
	/// <param name="n">Normal defining the orthonormal basis</param>
	/// <returns>A 3x3 matrix converting from the orthonormal base to world space</returns>
	static inline glm::mat3 orthonormalToWorld(const glm::vec3& n) {
		float sign = std::copysignf(1.0f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		const glm::vec3 b1(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		const glm::vec3 b2(b, sign + n.y * n.y * a, -n.y);
		glm::mat3 A;
		A[0][0] = b1.x;
		A[0][1] = b2.x;
		A[0][2] = n.x;

		A[1][0] = b1.y;
		A[1][1] = b2.y;
		A[1][2] = n.y;

		A[2][0] = b1.z;
		A[2][1] = b2.z;
		A[2][2] = n.z;
		return A;
	}

	/// <summary>Compute a matrix converting points in the orthonormal space defined by n to world coordinates</summary>
	/// <remark>Implemented using https://graphics.pixar.com/library/OrthonormalB/paper.pdf </remark>
	/// <param name="n">Normal defining the orthonormal basis</param>
	/// <returns>A 4x4 matrix converting from the orthonormal base to world space</returns>
	static inline glm::mat4 orthonormalToWorld(const glm::vec4& n) {
		float sign = std::copysignf(1.0f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		const glm::vec3 b1(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		const glm::vec3 b2(b, sign + n.y * n.y * a, -n.y);
		glm::mat4 A(1);
		A[0][0] = b1.x;
		A[0][1] = b2.x;
		A[0][2] = n.x;

		A[1][0] = b1.y;
		A[1][1] = b2.y;
		A[1][2] = n.y;

		A[2][0] = b1.z;
		A[2][1] = b2.z;
		A[2][2] = n.z;
		return A;
	}

	static inline glm::mat4 zAxistoWorld(const glm::vec4& z) {
		const glm::vec3 _tmp = glm::normalize(glm::vec3(z));
		const glm::vec3 v(_tmp.y, -_tmp.x, 0);
		const float angle = std::acos(_tmp.z);
		if (angle < 1e-6f || glm::dot(v, v) < 1e-4f) {
			return glm::mat4(1);
		}
		return glm::rotate(angle, v);
	}

	static inline glm::mat4 zAxistoWorld(const glm::vec3& z) {
		const glm::vec3 _tmp = glm::normalize(z);
		const glm::vec3 v(_tmp.y, -_tmp.x, 0);
		const float angle = std::acos(_tmp.z);
		if (angle < 1e-6f || glm::dot(v, v) < 1e-4f) {
			return glm::mat4(1);
		}
		return glm::rotate(angle, v);
	}

	static inline glm::vec4 getAxis4(int dimension) {
		glm::vec4 a(0);
		a[dimension] = 1.0f;
		return a;
	}
	
	static inline glm::vec3 getAxis3(int dimension) {
		glm::vec3 a(0);
		a[dimension] = 1.0f;
		return a;
	}

	/// <summary>Computes the closest point on line (p, u) to line (q, v)</summary>
	/// <param name="p">A point on the line (p, u)</param>
	/// <param name="u">The direction of line (p, u)</param>
	/// <param name="q">A point on the line (q, v)</param>
	/// <param name="v">The direction of line (q, v)</param>
	/// <returns>The point on line (p, u) that is closest to line (q, v).</returns>
	static inline glm::vec4 getClosestPointOnLine(const glm::vec4& p, const glm::vec4& u, const glm::vec4& q, const glm::vec4& v) {
		const glm::vec4 w = p - q;
		const float a = glm::dot(u, u);
		const float b = glm::dot(u, v);
		const float c = glm::dot(v, v);
		const float d = glm::dot(u, w);
		const float e = glm::dot(v, w);
	
		glm::pi<float>();
		const float denom = a * c - b * b;
		if (std::abs(denom) < 1e-16f) {
			return glm::vec4(std::numeric_limits<float>::infinity());
		}
		const float t = (b * e - c * d) / denom;
		return p + t * u;
	}

	/// <summary>Computes the intersection point of ray (p, u) with a given plane</summary>
	/// <param name="p">A point on the line (p, u)</param>
	/// <param name="u">The direction of line (p, u)</param>
	/// <param name="q">A point on the plane</param>
	/// <param name="n">Normal of the plane</param>
	/// <returns>The intersection point between the plane and the ray (p, u).</returns>
	static inline glm::vec4 pointPlaneIntersection(const glm::vec4& p, const glm::vec4& u, const glm::vec4& q, const glm::vec4& n) {
		const float denom = glm::dot(u, n);
		if (std::abs(denom) < 1e-16f) {
			return glm::vec4(std::numeric_limits<float>::infinity());
		}
		const float t = (glm::dot(n, q) - glm::dot(n, p)) / denom;
		return p + t * u;
	}

	/// <summary>Computes the closest point on a unit circle on a given plane to the ray (p, u)</summary>
	/// <param name="p">A point on the line (p, u)</param>
	/// <param name="u">The direction of line (p, u)</param>
	/// <param name="q">Center of the circle</param>
	/// <param name="n">Normal of the plane containing the circle</param>
	/// <returns>The intersection point between the plane and the ray (p, u).</returns>
	static inline glm::vec4 closestPointOnUnitCircle(const glm::vec4& p, const glm::vec4& u, const glm::vec4& q, const glm::vec4& n) {
		const float denom = glm::dot(u, n);
		if (std::abs(denom) < 1e-16f) {
			return glm::vec4(std::numeric_limits<float>::infinity());
		}
		const float t = (glm::dot(n, q) - glm::dot(n, p)) / denom;
		const glm::vec4 pointOnPlane = p + t * u;
		return q + glm::normalize(pointOnPlane - q);
	}

}