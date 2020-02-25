#pragma once
#include "DebuggablePolyConnectivity.h"

namespace OpenMesh {
	
DebuggablePolyConnectivity::DebuggablePolyConnectivity() {
	debugBreakCallback_ = []() {
		__debugbreak();
	};
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(const std::vector<VertexHandle>& _vhandles)
{
	for (VertexHandle vh : _vhandles) {
		checkBreakPoints(vh, OpenMeshFunction::add_face);
	}
	return PolyConnectivity::add_face(_vhandles);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(const std::vector<SmartVertexHandle>& _vhandles)
{
	for (VertexHandle vh : _vhandles) {
		checkBreakPoints(vh, OpenMeshFunction::add_face);
	}
	return PolyConnectivity::add_face(_vhandles);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2)
{
	checkBreakPoints(_vh0, OpenMeshFunction::add_face);
	checkBreakPoints(_vh1, OpenMeshFunction::add_face);
	checkBreakPoints(_vh2, OpenMeshFunction::add_face);
	return PolyConnectivity::add_face(_vh0, _vh1, _vh2);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2, VertexHandle _vh3)
{
	checkBreakPoints(_vh0, OpenMeshFunction::add_face);
	checkBreakPoints(_vh1, OpenMeshFunction::add_face);
	checkBreakPoints(_vh2, OpenMeshFunction::add_face);
	checkBreakPoints(_vh3, OpenMeshFunction::add_face);
	return PolyConnectivity::add_face(_vh0, _vh1, _vh2, _vh3);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(const VertexHandle* _vhandles, size_t _vhs_size)
{
	for (size_t i = 0; i < _vhs_size; ++i) {
		checkBreakPoints(_vhandles[i], OpenMeshFunction::add_face);

	}
	return PolyConnectivity::add_face(_vhandles, _vhs_size);
}

bool DebuggablePolyConnectivity::is_collapse_ok(HalfedgeHandle _he)
{
	checkBreakPoints(_he, OpenMeshFunction::is_collapse_ok);
	return PolyConnectivity::is_collapse_ok(_he);
}

void DebuggablePolyConnectivity::delete_vertex(VertexHandle _vh, bool _delete_isolated_vertices)
{
	checkBreakPoints(_vh, OpenMeshFunction::delete_vertex);
	PolyConnectivity::delete_vertex(_vh, _delete_isolated_vertices);
}

void DebuggablePolyConnectivity::delete_edge(EdgeHandle _eh, bool _delete_isolated_vertices)
{
	checkBreakPoints(_eh, OpenMeshFunction::delete_edge);
	PolyConnectivity::delete_edge(_eh, _delete_isolated_vertices);
}

void DebuggablePolyConnectivity::delete_face(FaceHandle _fh, bool _delete_isolated_vertices)
{
	checkBreakPoints(_fh, OpenMeshFunction::delete_face);
	PolyConnectivity::delete_face(_fh, _delete_isolated_vertices);
}

HalfedgeHandle DebuggablePolyConnectivity::next_halfedge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::next_halfedge_handle);
	return ArrayKernel::next_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::prev_halfedge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::prev_halfedge_handle);
	return ArrayKernel::prev_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::opposite_halfedge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::opposite_halfedge_handle);
	return ArrayKernel::opposite_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::ccw_rotated_halfedge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::ccw_rotated_halfedge_handle);
	return ArrayKernel::ccw_rotated_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::cw_rotated_halfedge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::cw_rotated_halfedge_handle);
	return ArrayKernel::cw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::next_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::next_halfedge_handle);
	return PolyConnectivity::next_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::prev_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::prev_halfedge_handle);
	return PolyConnectivity::prev_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::opposite_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::opposite_halfedge_handle);
	return PolyConnectivity::opposite_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::ccw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::ccw_rotated_halfedge_handle);
	return PolyConnectivity::ccw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::cw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::cw_rotated_halfedge_handle);
	return PolyConnectivity::cw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartEdgeHandle _eh, unsigned int _i) const
{
	checkBreakPoints(_eh, OpenMeshFunction::halfedge_handle);
	return PolyConnectivity::halfedge_handle(_eh, _i);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(EdgeHandle _eh, unsigned int _i) const
{
	checkBreakPoints(_eh, OpenMeshFunction::halfedge_handle);
	return ArrayKernel::halfedge_handle(_eh, _i);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartFaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::halfedge_handle);
	return PolyConnectivity::halfedge_handle(_fh);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::halfedge_handle);
	return PolyConnectivity::halfedge_handle(_fh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartVertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::halfedge_handle);
	return PolyConnectivity::halfedge_handle(_vh);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::halfedge_handle);
	return PolyConnectivity::halfedge_handle(_vh);
}

SmartEdgeHandle DebuggablePolyConnectivity::edge_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::edge_handle);
	return PolyConnectivity::edge_handle(_heh);
}

EdgeHandle DebuggablePolyConnectivity::edge_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::edge_handle);
	return PolyConnectivity::edge_handle(_heh);
}

SmartFaceHandle DebuggablePolyConnectivity::face_handle(SmartHalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::face_handle);
	return PolyConnectivity::face_handle(_heh);
}

FaceHandle DebuggablePolyConnectivity::face_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::face_handle);
	return PolyConnectivity::face_handle(_heh);
}

PolyConnectivity::ConstVertexVertexRange DebuggablePolyConnectivity::vv_range(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::vv_range);
	return PolyConnectivity::vv_range(_vh);
}

PolyConnectivity::ConstVertexIHalfedgeRange DebuggablePolyConnectivity::vih_range(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::vih_range);
	return PolyConnectivity::vih_range(_vh);
}

PolyConnectivity::ConstVertexOHalfedgeRange DebuggablePolyConnectivity::voh_range(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::voh_range);
	return PolyConnectivity::voh_range(_vh);
}

PolyConnectivity::ConstVertexEdgeRange DebuggablePolyConnectivity::ve_range(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::ve_range);
	return PolyConnectivity::ve_range(_vh);
}

PolyConnectivity::ConstVertexFaceRange DebuggablePolyConnectivity::vf_range(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::vf_range);;
	return PolyConnectivity::vf_range(_vh);
}

PolyConnectivity::ConstFaceVertexRange DebuggablePolyConnectivity::fv_range(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::fv_range);
	return PolyConnectivity::fv_range(_fh);
}

PolyConnectivity::ConstFaceHalfedgeRange DebuggablePolyConnectivity::fh_range(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::fh_range);
	return PolyConnectivity::fh_range(_fh);
}

PolyConnectivity::ConstFaceEdgeRange DebuggablePolyConnectivity::fe_range(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::fe_range);
	return PolyConnectivity::fe_range(_fh);
}

PolyConnectivity::ConstFaceFaceRange DebuggablePolyConnectivity::ff_range(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::ff_range);
	return PolyConnectivity::ff_range(_fh);
}

bool DebuggablePolyConnectivity::is_boundary(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::is_boundary);
	return PolyConnectivity::is_boundary(_heh);
}

bool DebuggablePolyConnectivity::is_boundary(EdgeHandle _eh) const
{
	checkBreakPoints(_eh, OpenMeshFunction::is_boundary);
	return PolyConnectivity::is_boundary(_eh);
}

bool DebuggablePolyConnectivity::is_boundary(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::is_boundary);
	return PolyConnectivity::is_boundary(_vh);
}

bool DebuggablePolyConnectivity::is_boundary(FaceHandle _fh, bool _check_vertex) const
{
	checkBreakPoints(_fh, OpenMeshFunction::is_boundary);
	return PolyConnectivity::is_boundary(_fh, _check_vertex);
}

bool DebuggablePolyConnectivity::is_manifold(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::is_boundary);
	return PolyConnectivity::is_manifold(_vh);
}

FaceHandle DebuggablePolyConnectivity::opposite_face_handle(HalfedgeHandle _heh) const
{
	checkBreakPoints(_heh, OpenMeshFunction::opposite_face_handle);
	return PolyConnectivity::opposite_face_handle(_heh);
}

void DebuggablePolyConnectivity::adjust_outgoing_halfedge(VertexHandle _vh)
{
	checkBreakPoints(_vh, OpenMeshFunction::adjust_outgoing_halfedge);
	return PolyConnectivity::adjust_outgoing_halfedge(_vh);
}

HalfedgeHandle DebuggablePolyConnectivity::find_halfedge(VertexHandle _start_vh, VertexHandle _end_vh) const
{
	checkBreakPoints(_start_vh, OpenMeshFunction::find_halfedge);
	checkBreakPoints(_end_vh, OpenMeshFunction::find_halfedge);
	return PolyConnectivity::find_halfedge(_start_vh, _end_vh);
}

uint DebuggablePolyConnectivity::valence(VertexHandle _vh) const
{
	checkBreakPoints(_vh, OpenMeshFunction::valence);
	return PolyConnectivity::valence(_vh);
}

uint DebuggablePolyConnectivity::valence(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::valence);
	return PolyConnectivity::valence(_fh);
}

void DebuggablePolyConnectivity::collapse(HalfedgeHandle _heh)
{
	checkBreakPoints(_heh, OpenMeshFunction::collapse);
	return PolyConnectivity::collapse(_heh);
}

bool DebuggablePolyConnectivity::is_simple_link(EdgeHandle _eh) const
{
	checkBreakPoints(_eh, OpenMeshFunction::is_simple_link);
	return PolyConnectivity::is_simple_link(_eh);
}

bool DebuggablePolyConnectivity::is_simply_connected(FaceHandle _fh) const
{
	checkBreakPoints(_fh, OpenMeshFunction::is_simply_connected);
	return PolyConnectivity::is_simply_connected(_fh);
}

FaceHandle DebuggablePolyConnectivity::remove_edge(EdgeHandle _eh)
{
	checkBreakPoints(_eh, OpenMeshFunction::delete_edge);
	return PolyConnectivity::remove_edge(_eh);
}

void DebuggablePolyConnectivity::reinsert_edge(EdgeHandle _eh)
{
	checkBreakPoints(_eh, OpenMeshFunction::insert_edge);
	return PolyConnectivity::reinsert_edge(_eh);
}

HalfedgeHandle DebuggablePolyConnectivity::insert_edge(HalfedgeHandle _prev_heh, HalfedgeHandle _next_heh)
{
	checkBreakPoints(_prev_heh, OpenMeshFunction::insert_edge);
	checkBreakPoints(_next_heh, OpenMeshFunction::insert_edge);
	return PolyConnectivity::insert_edge(_prev_heh, _next_heh);
}

void DebuggablePolyConnectivity::split(FaceHandle _fh, VertexHandle _vh)
{
	checkBreakPoints(_fh, OpenMeshFunction::split);
	checkBreakPoints(_vh, OpenMeshFunction::split);
	PolyConnectivity::split(_fh, _vh);
}

void DebuggablePolyConnectivity::split_copy(FaceHandle _fh, VertexHandle _vh)
{
	checkBreakPoints(_fh, OpenMeshFunction::split);
	checkBreakPoints(_vh, OpenMeshFunction::split);
	PolyConnectivity::split_copy(_fh, _vh);
}

void DebuggablePolyConnectivity::triangulate(FaceHandle _fh)
{
	checkBreakPoints(_fh, OpenMeshFunction::triangulate);
	PolyConnectivity::triangulate(_fh);
}

void DebuggablePolyConnectivity::split_edge(EdgeHandle _eh, VertexHandle _vh)
{
	checkBreakPoints(_eh, OpenMeshFunction::split);
	checkBreakPoints(_vh, OpenMeshFunction::split);
	PolyConnectivity::split_edge(_eh, _vh);
}

void DebuggablePolyConnectivity::split_edge_copy(EdgeHandle _eh, VertexHandle _vh)
{
	checkBreakPoints(_eh, OpenMeshFunction::split);
	checkBreakPoints(_vh, OpenMeshFunction::split);
	PolyConnectivity::split_edge_copy(_eh, _vh);
}

}