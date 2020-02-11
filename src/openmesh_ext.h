#pragma once

#include <glm/glm.hpp>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>

namespace OpenMesh {
	template<int d, typename Scalar>
	struct vector_traits<glm::vec<d, Scalar>>
	{
		/// Type of the vector class
		typedef typename std::vector<glm::vec<d, Scalar>> vector_type;

		/// Type of the scalar value
		typedef typename Scalar value_type;

		/// size/dimension of the vector
		static const size_t size_ = d;

		/// size/dimension of the vector
		static size_t size() { return size_; }
	};

	template<int d, typename Scalar>
	float norm(const glm::vec<d, Scalar>& _v) {
		return std::sqrt(glm::dot(_v, _v));
	}

	template<typename Scalar, int DIM, typename OtherScalar>
	glm::vec<DIM, Scalar>& vectorize(glm::vec<DIM, Scalar>& _v, OtherScalar const& _val) {
		_v = glm::vec<DIM, Scalar>(Scalar(_val));
		return _v;
	}

	template<typename T, int d>
	struct GLMTraits : OpenMesh::DefaultTraits {
		typedef glm::vec<d, T> Point;
		typedef glm::vec<d, T> Normal;
	};

	typedef TriMesh_ArrayKernelT<GLMTraits<float, 3>>  TriangleMesh3f;
	typedef TriMesh_ArrayKernelT<GLMTraits<double, 3>> TriangleMesh3d;
}
