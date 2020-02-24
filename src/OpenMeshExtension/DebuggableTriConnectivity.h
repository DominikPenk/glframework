#pragma once

#include "DebuggablePolyConnectivity.h"

namespace OpenMesh {
class DebuggableTriConnectivity : public DebuggablePolyConnectivity {
public:
	DebuggableTriConnectivity() {}
	virtual ~DebuggableTriConnectivity() { }

	inline static bool is_triangles()
	{
		return true;
	}	

	inline void assign_connectivity(const DebuggableTriConnectivity& _other)
	{
		DebuggablePolyConnectivity::assign_connectivity(_other);
	}

	inline void assign_connectivity(const DebuggablePolyConnectivity& _other)
	{
		DebuggablePolyConnectivity::assign_connectivity(_other);
		PolyConnectivity::triangulate();
	}

	/** \name Addding items to a mesh
	*/

	//@{

	/** \brief Add a face with arbitrary valence to the triangle mesh
	 *
	 * Override OpenMesh::Mesh::PolyMeshT::add_face(). Faces that aren't
	 * triangles will be triangulated and added. In this case an
	 * invalid face handle will be returned.
	 *
	 *
	 * */
	SmartFaceHandle add_face(const VertexHandle* _vhandles, size_t _vhs_size);

	/** \brief Add a face with arbitrary valence to the triangle mesh
	   *
	   * Override OpenMesh::Mesh::PolyMeshT::add_face(). Faces that aren't
	   * triangles will be triangulated and added. In this case an
	   * invalid face handle will be returned.
	   *
	   *
	   * */
	SmartFaceHandle add_face(const std::vector<VertexHandle>& _vhandles);

	/** \brief Add a face with arbitrary valence to the triangle mesh
	   *
	   * Override OpenMesh::Mesh::PolyMeshT::add_face(). Faces that aren't
	   * triangles will be triangulated and added. In this case an
	   * invalid face handle will be returned.
	   *
	   *
	   * */
	SmartFaceHandle add_face(const std::vector<SmartVertexHandle>& _vhandles);

	/** \brief Add a face to the mesh (triangle)
	 *
	 * This function adds a triangle to the mesh. The triangle is passed directly
	 * to the underlying PolyConnectivity as we don't explicitly need to triangulate something.
	 *
	 * @param _vh0 VertexHandle 1
	 * @param _vh1 VertexHandle 2
	 * @param _vh2 VertexHandle 3
	 * @return FaceHandle of the added face (invalid, if the operation failed)
	 */
	SmartFaceHandle add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2);

	//@}

	/** Returns the opposite vertex to the halfedge _heh in the face
		referenced by _heh returns InvalidVertexHandle if the _heh is
		boundary  */
	inline VertexHandle opposite_vh(HalfedgeHandle _heh) const
	{
		return is_boundary(_heh) ? InvalidVertexHandle :
			to_vertex_handle(next_halfedge_handle(_heh));
	}

	/** Returns the opposite vertex to the opposite halfedge of _heh in
		the face referenced by it returns InvalidVertexHandle if the
		opposite halfedge is boundary  */
	VertexHandle opposite_he_opposite_vh(HalfedgeHandle _heh) const
	{
		return opposite_vh(opposite_halfedge_handle(_heh));
	}

	/** \name Topology modifying operators
	*/
	//@{


	/** Returns whether collapsing halfedge _heh is ok or would lead to
		topological inconsistencies.
		\attention This method need the Attributes::Status attribute and
		changes the \em tagged bit.  */
	bool is_collapse_ok(HalfedgeHandle _heh);

	/// Vertex Split: inverse operation to collapse().
	HalfedgeHandle vertex_split(VertexHandle v0, VertexHandle v1,
		VertexHandle vl, VertexHandle vr);

	/// Check whether flipping _eh is topologically correct.
	bool is_flip_ok(EdgeHandle _eh) const;

	/** Flip edge _eh.
		Check for topological correctness first using is_flip_ok(). */
	void flip(EdgeHandle _eh);


	/** \brief Edge split (= 2-to-4 split)
	 *
	 *
	 * The function will introduce two new faces ( non-boundary case) or
	 * one additional face (if edge is boundary)
	 *
	 * \note The properties of the new edges, halfedges, and faces will be undefined!
	 *
	 * @param _eh Edge handle that should be split
	 * @param _vh Vertex handle that will be inserted at the edge
	 */
	void split(EdgeHandle _eh, VertexHandle _vh);

	/** \brief Edge split (= 2-to-4 split)
	   *
	   *
	   * The function will introduce two new faces ( non-boundary case) or
	   * one additional face (if edge is boundary)
	   *
	   * \note The properties of the new edges, halfedges, and faces will be undefined!
	   *
	   * \note This is an override to prevent a direct call to PolyConnectivity split_edge,
	   *       which would introduce a singular vertex with valence 2 which is not allowed
	   *       on TriMeshes
	   *
	   * @param _eh Edge handle that should be split
	   * @param _vh Vertex handle that will be inserted at the edge
	   */
	inline void split_edge(EdgeHandle _eh, VertexHandle _vh) { DebuggableTriConnectivity::split(_eh, _vh); }

	/** \brief Edge split (= 2-to-4 split)
	 *
	 * The function will introduce two new faces ( non-boundary case) or
	 * one additional face (if edge is boundary)
	 *
	 * \note The properties of the new edges and faces will be adjusted to the
	 *       properties of the original edge and face
	 * \note The properties of the new halfedges will be undefined
	 *
	 * @param _eh Edge handle that should be split
	 * @param _vh Vertex handle that will be inserted at the edge
	 */
	void split_copy(EdgeHandle _eh, VertexHandle _vh);

	/** \brief Edge split (= 2-to-4 split)
	 *
	 * The function will introduce two new faces ( non-boundary case) or
	 * one additional face (if edge is boundary)
	 *
	 * \note The properties of the new edges and faces will be adjusted to the
	 *       properties of the original edge and face
	 * \note The properties of the new halfedges will be undefined
	 *
	 * \note This is an override to prevent a direct call to PolyConnectivity split_edge_copy,
	 *       which would introduce a singular vertex with valence 2 which is not allowed
	 *       on TriMeshes
	 *
	 * @param _eh Edge handle that should be split
	 * @param _vh Vertex handle that will be inserted at the edge
	 */
	inline void split_edge_copy(EdgeHandle _eh, VertexHandle _vh) { DebuggableTriConnectivity::split_copy(_eh, _vh); }

	/** \brief Face split (= 1-to-3) split, calls corresponding PolyMeshT function).
	 *
	 * @param _fh Face handle that should be split
	 * @param _vh Vertex handle that will be inserted at the face
	 */
	inline void split(FaceHandle _fh, VertexHandle _vh)
	{
		PolyConnectivity::split(_fh, _vh);
	}

	/** \brief Face split (= 1-to-3) split, calls corresponding PolyMeshT function).
	 *
	 * @param _fh Face handle that should be split
	 * @param _vh Vertex handle that will be inserted at the face
	 */
	inline void split_copy(FaceHandle _fh, VertexHandle _vh)
	{
		PolyConnectivity::split_copy(_fh, _vh);
	}

	//@}

private:
	/// Helper for vertex split
	HalfedgeHandle insert_loop(HalfedgeHandle _hh);
	/// Helper for vertex split
	HalfedgeHandle insert_edge(VertexHandle _vh,
		HalfedgeHandle _h0, HalfedgeHandle _h1);
};
}