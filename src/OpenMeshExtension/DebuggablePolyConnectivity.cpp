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
	if (std::any_of(_vhandles.begin(), _vhandles.end(), [=](const VertexHandle _vh) { return shouldBreak(_vh); }))
		debugBreakCallback_();
	return PolyConnectivity::add_face(_vhandles);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(const std::vector<SmartVertexHandle>& _vhandles)
{
	if (std::any_of(_vhandles.begin(), _vhandles.end(), [=](const SmartVertexHandle _vh) { return shouldBreak(_vh); }))
		debugBreakCallback_();
	return PolyConnectivity::add_face(_vhandles);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2)
{
	if (shouldBreak(_vh0) || shouldBreak(_vh1) || shouldBreak(_vh2)) debugBreakCallback_();
	return PolyConnectivity::add_face(_vh0, _vh1, _vh2);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(VertexHandle _vh0, VertexHandle _vh1, VertexHandle _vh2, VertexHandle _vh3)
{
	if (shouldBreak(_vh0) || shouldBreak(_vh1) || shouldBreak(_vh2) || shouldBreak(_vh3)) debugBreakCallback_();
	return PolyConnectivity::add_face(_vh0, _vh1, _vh2, _vh3);
}

SmartFaceHandle DebuggablePolyConnectivity::add_face(const VertexHandle* _vhandles, size_t _vhs_size)
{
	if (std::any_of(_vhandles, _vhandles + _vhs_size, [=](const VertexHandle _vh) { return shouldBreak(_vh); }))
		debugBreakCallback_();
	return PolyConnectivity::add_face(_vhandles, _vhs_size);
}

bool DebuggablePolyConnectivity::is_collapse_ok(HalfedgeHandle _he)
{
	if (shouldBreak(_he)) debugBreakCallback_();
	return PolyConnectivity::is_collapse_ok(_he);
}

void DebuggablePolyConnectivity::delete_vertex(VertexHandle _vh, bool _delete_isolated_vertices)
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	PolyConnectivity::delete_vertex(_vh, _delete_isolated_vertices);
}

void DebuggablePolyConnectivity::delete_edge(EdgeHandle _eh, bool _delete_isolated_vertices)
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	PolyConnectivity::delete_edge(_eh, _delete_isolated_vertices);
}

void DebuggablePolyConnectivity::delete_face(FaceHandle _fh, bool _delete_isolated_vertices)
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	PolyConnectivity::delete_face(_fh, _delete_isolated_vertices);
}

HalfedgeHandle DebuggablePolyConnectivity::next_halfedge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return ArrayKernel::next_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::prev_halfedge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return ArrayKernel::prev_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::opposite_halfedge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return ArrayKernel::opposite_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::ccw_rotated_halfedge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return ArrayKernel::ccw_rotated_halfedge_handle(_heh);
}

HalfedgeHandle DebuggablePolyConnectivity::cw_rotated_halfedge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return ArrayKernel::cw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::next_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::next_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::prev_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::prev_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::opposite_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::opposite_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::ccw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::ccw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::cw_rotated_halfedge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::cw_rotated_halfedge_handle(_heh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartEdgeHandle _eh, unsigned int _i) const
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return PolyConnectivity::halfedge_handle(_eh, _i);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(EdgeHandle _eh, unsigned int _i) const
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return ArrayKernel::halfedge_handle(_eh, _i);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartFaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::halfedge_handle(_fh);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::halfedge_handle(_fh);
}

SmartHalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(SmartVertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::halfedge_handle(_vh);
}

HalfedgeHandle DebuggablePolyConnectivity::halfedge_handle(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::halfedge_handle(_vh);
}

SmartEdgeHandle DebuggablePolyConnectivity::edge_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::edge_handle(_heh);
}

EdgeHandle DebuggablePolyConnectivity::edge_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::edge_handle(_heh);
}

SmartFaceHandle DebuggablePolyConnectivity::face_handle(SmartHalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::face_handle(_heh);
}

FaceHandle DebuggablePolyConnectivity::face_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::face_handle(_heh);
}

PolyConnectivity::ConstVertexVertexRange DebuggablePolyConnectivity::vv_range(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::vv_range(_vh);
}

PolyConnectivity::ConstVertexIHalfedgeRange DebuggablePolyConnectivity::vih_range(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::vih_range(_vh);
}

PolyConnectivity::ConstVertexOHalfedgeRange DebuggablePolyConnectivity::voh_range(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::voh_range(_vh);
}

PolyConnectivity::ConstVertexEdgeRange DebuggablePolyConnectivity::ve_range(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::ve_range(_vh);
}

PolyConnectivity::ConstVertexFaceRange DebuggablePolyConnectivity::vf_range(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::vf_range(_vh);
}

PolyConnectivity::ConstFaceVertexRange DebuggablePolyConnectivity::fv_range(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::fv_range(_fh);
}

PolyConnectivity::ConstFaceHalfedgeRange DebuggablePolyConnectivity::fh_range(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::fh_range(_fh);
}

PolyConnectivity::ConstFaceEdgeRange DebuggablePolyConnectivity::fe_range(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::fe_range(_fh);
}

PolyConnectivity::ConstFaceFaceRange DebuggablePolyConnectivity::ff_range(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::ff_range(_fh);
}

bool DebuggablePolyConnectivity::is_boundary(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::is_boundary(_heh);
}

bool DebuggablePolyConnectivity::is_boundary(EdgeHandle _eh) const
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return PolyConnectivity::is_boundary(_eh);
}

bool DebuggablePolyConnectivity::is_boundary(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::is_boundary(_vh);
}

bool DebuggablePolyConnectivity::is_boundary(FaceHandle _fh, bool _check_vertex) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::is_boundary(_fh, _check_vertex);
}

bool DebuggablePolyConnectivity::is_manifold(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::is_manifold(_vh);
}

FaceHandle DebuggablePolyConnectivity::opposite_face_handle(HalfedgeHandle _heh) const
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::opposite_face_handle(_heh);
}

void DebuggablePolyConnectivity::adjust_outgoing_halfedge(VertexHandle _vh)
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::adjust_outgoing_halfedge(_vh);
}

HalfedgeHandle DebuggablePolyConnectivity::find_halfedge(VertexHandle _start_vh, VertexHandle _end_vh) const
{
	if (shouldBreak(_start_vh) || shouldBreak(_end_vh)) debugBreakCallback_();
	return PolyConnectivity::find_halfedge(_start_vh, _end_vh);
}

uint DebuggablePolyConnectivity::valence(VertexHandle _vh) const
{
	if (shouldBreak(_vh)) debugBreakCallback_();
	return PolyConnectivity::valence(_vh);
}

uint DebuggablePolyConnectivity::valence(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::valence(_fh);
}

void DebuggablePolyConnectivity::collapse(HalfedgeHandle _heh)
{
	if (shouldBreak(_heh)) debugBreakCallback_();
	return PolyConnectivity::collapse(_heh);
}

bool DebuggablePolyConnectivity::is_simple_link(EdgeHandle _eh) const
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return PolyConnectivity::is_simple_link(_eh);
}

bool DebuggablePolyConnectivity::is_simply_connected(FaceHandle _fh) const
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	return PolyConnectivity::is_simply_connected(_fh);
}

FaceHandle DebuggablePolyConnectivity::remove_edge(EdgeHandle _eh)
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return PolyConnectivity::remove_edge(_eh);
}

void DebuggablePolyConnectivity::reinsert_edge(EdgeHandle _eh)
{
	if (shouldBreak(_eh)) debugBreakCallback_();
	return PolyConnectivity::reinsert_edge(_eh);
}

HalfedgeHandle DebuggablePolyConnectivity::insert_edge(HalfedgeHandle _prev_heh, HalfedgeHandle _next_heh)
{
	if (shouldBreak(_prev_heh) || shouldBreak(_next_heh)) debugBreakCallback_();
	return PolyConnectivity::insert_edge(_prev_heh, _next_heh);
}

void DebuggablePolyConnectivity::split(FaceHandle _fh, VertexHandle _vh)
{
	if (shouldBreak(_fh) || shouldBreak(_vh)) debugBreakCallback_();
	PolyConnectivity::split(_fh, _vh);
}

void DebuggablePolyConnectivity::split_copy(FaceHandle _fh, VertexHandle _vh)
{
	if (shouldBreak(_fh) || shouldBreak(_vh)) debugBreakCallback_();
	PolyConnectivity::split_copy(_fh, _vh);
}

void DebuggablePolyConnectivity::triangulate(FaceHandle _fh)
{
	if (shouldBreak(_fh)) debugBreakCallback_();
	PolyConnectivity::triangulate(_fh);
}

void DebuggablePolyConnectivity::split_edge(EdgeHandle _eh, VertexHandle _vh)
{
	if (shouldBreak(_eh) || shouldBreak(_vh)) debugBreakCallback_();
	PolyConnectivity::split_edge(_eh, _vh);
}

void DebuggablePolyConnectivity::split_edge_copy(EdgeHandle _eh, VertexHandle _vh)
{
	if (shouldBreak(_eh) || shouldBreak(_vh)) debugBreakCallback_();
	PolyConnectivity::split_edge_copy(_eh, _vh);
}

}