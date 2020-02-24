#pragma once

#include "DebuggableMixin.h"

#include <OpenMesh/Core/Mesh/TriConnectivity.hh>

namespace OpenMesh {
	
class DebuggablePolyConnectivity : public PolyConnectivity, public DebuggalbeMixin {
public:
	DebuggablePolyConnectivity();
	virtual ~DebuggablePolyConnectivity() { }

	SmartFaceHandle add_face(const std::vector<VertexHandle>& _vhandles);

	SmartFaceHandle add_face(const std::vector<SmartVertexHandle>& _vhandles);

	SmartFaceHandle add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2);

	SmartFaceHandle add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2, VertexHandle _vh3);

	SmartFaceHandle add_face(const VertexHandle* _vhandles, size_t _vhs_size);

	bool is_collapse_ok(HalfedgeHandle _he);

	void delete_vertex(VertexHandle _vh, bool _delete_isolated_vertices = true);

	void delete_edge(EdgeHandle _eh, bool _delete_isolated_vertices = true);

	void delete_face(FaceHandle _fh, bool _delete_isolated_vertices = true);

	// "Dump" ArrayKernel halfedge access 
	HalfedgeHandle next_halfedge_handle(HalfedgeHandle _heh) const;
	HalfedgeHandle prev_halfedge_handle(HalfedgeHandle _heh) const;
	HalfedgeHandle opposite_halfedge_handle(HalfedgeHandle _heh) const;
	HalfedgeHandle ccw_rotated_halfedge_handle(HalfedgeHandle _heh) const;
	HalfedgeHandle cw_rotated_halfedge_handle(HalfedgeHandle _heh) const;

	// "Smart" Polyconnectivity halfedge access
	SmartHalfedgeHandle next_halfedge_handle(SmartHalfedgeHandle _heh) const;
	SmartHalfedgeHandle prev_halfedge_handle(SmartHalfedgeHandle _heh) const;
	SmartHalfedgeHandle opposite_halfedge_handle(SmartHalfedgeHandle _heh) const;
	SmartHalfedgeHandle ccw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const;
	SmartHalfedgeHandle cw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const;

	// "Dump" ArrayKernel per element access
	HalfedgeHandle halfedge_handle(EdgeHandle _eh, unsigned int _i) const;
	HalfedgeHandle halfedge_handle(FaceHandle _fh) const;
	HalfedgeHandle halfedge_handle(VertexHandle _vh) const;
	EdgeHandle edge_handle(HalfedgeHandle _heh) const;
	FaceHandle face_handle(HalfedgeHandle _heh) const;

	// "Smart" Polyconnectivity per element access
	SmartHalfedgeHandle halfedge_handle(SmartEdgeHandle _eh, unsigned int _i) const;
	SmartHalfedgeHandle halfedge_handle(SmartFaceHandle _fh) const;
	SmartHalfedgeHandle halfedge_handle(SmartVertexHandle _vh) const;
	SmartEdgeHandle edge_handle(SmartHalfedgeHandle _heh) const;
	SmartFaceHandle face_handle(SmartHalfedgeHandle _heh) const;


	PolyConnectivity::ConstVertexVertexRange vv_range(VertexHandle _vh) const;
	PolyConnectivity::ConstVertexIHalfedgeRange vih_range(VertexHandle _vh) const;
	PolyConnectivity::ConstVertexOHalfedgeRange voh_range(VertexHandle _vh) const;
	PolyConnectivity::ConstVertexEdgeRange ve_range(VertexHandle _vh) const;
	PolyConnectivity::ConstVertexFaceRange vf_range(VertexHandle _vh) const;
	PolyConnectivity::ConstFaceVertexRange fv_range(FaceHandle _fh) const;
	PolyConnectivity::ConstFaceHalfedgeRange fh_range(FaceHandle _fh) const;
	PolyConnectivity::ConstFaceEdgeRange fe_range(FaceHandle _fh) const;
	PolyConnectivity::ConstFaceFaceRange ff_range(FaceHandle _fh) const;

	// Circulators



	bool is_boundary(HalfedgeHandle _heh) const;
	bool is_boundary(EdgeHandle _eh) const;
	bool is_boundary(VertexHandle _vh) const;
	bool is_boundary(FaceHandle _fh, bool _check_vertex = false) const;

	bool is_manifold(VertexHandle _vh) const;

	FaceHandle opposite_face_handle(HalfedgeHandle _heh) const;
	void adjust_outgoing_halfedge(VertexHandle _vh);

	HalfedgeHandle find_halfedge(VertexHandle _start_vh, VertexHandle _end_vh) const;

	uint valence(VertexHandle _vh) const;
	uint valence(FaceHandle _fh) const;

	void collapse(HalfedgeHandle _heh);
	bool is_simple_link(EdgeHandle _eh) const;
	bool is_simply_connected(FaceHandle _fh) const;

	FaceHandle remove_edge(EdgeHandle _eh);

	void reinsert_edge(EdgeHandle _eh);

	HalfedgeHandle insert_edge(HalfedgeHandle _prev_heh, HalfedgeHandle _next_heh);

	void split(FaceHandle _fh, VertexHandle _vh); 
	void split_copy(FaceHandle _fh, VertexHandle _vh);

	void triangulate(FaceHandle _fh);

	void split_edge(EdgeHandle _eh, VertexHandle _vh);
	void split_edge_copy(EdgeHandle _eh, VertexHandle _vh);
};
}
