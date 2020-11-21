#pragma once

#include <gl/mesh.hpp>

namespace gl {

	class BSplineSurfaceMesh : public Mesh {
	public:
		BSplineSurfaceMesh();

		/// <summary>Construct a bspline from a linearized square grid of control points in rowmajor storage order.
		/// UV coordinates will be automatically generated such that the top left corner has uv (0, 0) and bottom right (1, 1).</summary>
		/// <parameter name="vertices">Control points of the BSpline in rowmajor storage order.</parameter>
		BSplineSurfaceMesh(const std::vector<Eigen::Vector3f>& vertices);

		/// <summary>Construct a regular spaced BSpline surface starting at (topLeft, 0)</summary>
		/// <param name="topLeft">XY-Coordinates of the top left control point</param>
		/// <param name="spacing">Spacing in x and y direction between control points</param>
		/// <param name="n">Number of control points in u and v direction</param>
		BSplineSurfaceMesh(const Eigen::Vector2f& topLeft, float spacing, int n);

		/// <summary>Construct a new Mesh from the given Heightmap</summary>
		/// <param name="map">Heightmap used to construct the mesh</param>
		BSplineSurfaceMesh(const Heightmap& map);

		/// <summary>Construct a new mesh loading a Bicubic Bspline from the given file</summary>
		/// <param name="path">Filepath to a spline surface file</param>
		BSplineSurfaceMesh(const std::string path);

		virtual void render(const Renderer* env) override;

		virtual void drawOutliner() override;

		virtual bool handleIO(const Renderer* env, ImGuiIO& io) override;

		float& height(int i) { return mPoints->at(i).z(); }

		glm::vec3 color;
		glm::vec4 colorControlMesh;
		glm::vec4 plane;

	private:
		enum class DisplayStyle {
			FLAT = 0,
			POSITION = 1,
			UV = 2,
			DIOPTER_LINEAR = 3,
			DIOPTER_HSV = 4,
			DISTANCE = 5
		};

		void constructControlPolygon();

		// Display varaibles
		bool mShowControlMesh;
		DisplayStyle mDisplayStyle;

		DrawBatch mBatchSpline;
		DrawBatch mBatchControlMesh;

		std::shared_ptr<VertexBufferObject<float, 3>> mPoints;
		std::shared_ptr<VertexBufferObject<float, 2>> mUVs;

		Region mRegion;
	};
}
