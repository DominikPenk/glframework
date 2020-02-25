#pragma once
#include <functional>
#include <OpenMesh/Core/Mesh/Handles.hh>

namespace OpenMesh {

enum class OpenMeshFunction {
	// AttribKernelIT functions
	point                          = 0,
	set_point                      = 1,
	normal                         = 2,
	set_normal                     = 3,
	color                          = 4,
	set_color                      = 5,
	texcoord1D                     = 6,
	set_texcoord1D		           = 7,
	texcoord2D			           = 8,
	set_texcoord2D		           = 9,
	texcoord3D			           = 10,
	set_texcoord3D   	           = 11,
	texture_index		           = 12,
	set_texture_index	           = 13,
	data				           = 14,
	const_data			           = 15,

	// PolyConnectivity functions
	add_face                       = 20,
	is_collapse_ok                 = 21,
	delete_vertex				   = 22,
	delete_edge					   = 23,
	delete_face					   = 24,
	next_halfedge_handle		   = 25,
	prev_halfedge_handle		   = 26,
	opposite_halfedge_handle	   = 27,
	ccw_rotated_halfedge_handle    = 28,
	cw_rotated_halfedge_handle	   = 29,
	halfedge_handle 			   = 30,
	edge_handle 				   = 31,
	face_handle 				   = 32,
	vv_range 					   = 33,
	vih_range					   = 34,
	voh_range					   = 35,
	ve_range					   = 36,
	vf_range					   = 37,
	fv_range					   = 38,
	fh_range					   = 39,
	fe_range					   = 40,
	ff_range					   = 41,
	is_boundary                    = 42,

	opposite_face_handle           = 43,
	adjust_outgoing_halfedge       = 44,
	find_halfedge                  = 45,
	valence                        = 46,

	collapse                       = 47,
	split                          = 48,
	is_simple_link                 = 49,
	is_simply_connected            = 50,
	insert_edge                    = 51,
	triangulate                    = 52
};

typedef typename std::function<bool(OpenMeshFunction fn)> Conditional;

static Conditional Always = [](OpenMeshFunction fn) { return true; };
static Conditional CustomAttributeAcces = [](OpenMeshFunction fn) {
	return fn == OpenMeshFunction::const_data || fn == OpenMeshFunction::data;
};
static Conditional DefaultAttributeAccess = [](OpenMeshFunction fn) {
	return fn < OpenMeshFunction::data;
};
static Conditional AttributeAccess = [](OpenMeshFunction fn) {
	return fn <= OpenMeshFunction::const_data;
};
static Conditional ElementAdd = [](OpenMeshFunction fn) {
	return fn == OpenMeshFunction::add_face;
};
static Conditional ElementRemove = [](OpenMeshFunction fn) {
	return fn >= OpenMeshFunction::delete_vertex && fn <= OpenMeshFunction::delete_face;
};
static Conditional HalfedgeTraversal = [](OpenMeshFunction fn) {
	return fn >= OpenMeshFunction::next_halfedge_handle && fn <= OpenMeshFunction::cw_rotated_halfedge_handle;
};
static Conditional ElementAcces = [](OpenMeshFunction fn) {
	return fn >= OpenMeshFunction::halfedge_handle && fn <= OpenMeshFunction::face_handle;
};
static Conditional RangeIteraor = [](OpenMeshFunction fn) {
	return fn >= OpenMeshFunction::vv_range && fn <= OpenMeshFunction::ff_range;
};



struct BreakPoint {
	
	Conditional condition;
	std::function<void()> callback;
};

struct DebuggalbeMixin {

	DebuggalbeMixin() : disableCheck(false) {
		debugBreakCallback_ = []() {
			__debugbreak();
		};
	}

	std::function<void()> debugBreakCallback_;

	inline bool shouldBreak(VertexHandle vh) const;
	inline bool shouldBreak(EdgeHandle eh) const;
	inline bool shouldBreak(HalfedgeHandle heh) const;
	inline bool shouldBreak(FaceHandle fh) const;

	inline void checkBreakPoints(VertexHandle vh, OpenMeshFunction fn) const;
	inline void checkBreakPoints(EdgeHandle eh, OpenMeshFunction fn) const;
	inline void checkBreakPoints(HalfedgeHandle heh, OpenMeshFunction fn) const;
	inline void checkBreakPoints(FaceHandle fh, OpenMeshFunction fn) const;

	inline void addBreakpoint(VertexHandle vh, const std::function<void()>& callback = []() { __debugbreak(); }, Conditional condition = Always);
	inline void addBreakpoint(EdgeHandle eh, const std::function<void()>& callback = []() { __debugbreak(); }, Conditional condition = Always);
	inline void addBreakpoint(HalfedgeHandle heh, const std::function<void()>& callback = []() { __debugbreak(); }, Conditional condition = Always);
	inline void addBreakpoint(FaceHandle fh, const std::function<void()>& callback = []() { __debugbreak(); }, Conditional condition = Always);

	inline void watch(VertexHandle vh);
	inline void watch(EdgeHandle eh);
	inline void watch(HalfedgeHandle heh);
	inline void watch(FaceHandle fh);

	inline void stopWatch(VertexHandle vh);
	inline void stopWatch(EdgeHandle eh);
	inline void stopWatch(HalfedgeHandle heh);
	inline void stopWatch(FaceHandle fh);

	bool disableCheck;

	std::vector<VertexHandle>                           watchedVertexHandles;
	std::vector<EdgeHandle>                             watchedEdgeHandles;
	std::vector<HalfedgeHandle>                         watchedHalfedgeHandles;
	std::vector<FaceHandle>                             watchedFaceHandles;

	std::unordered_multimap<VertexHandle, BreakPoint>   breakPointVertexHandles;
	std::unordered_multimap<EdgeHandle, BreakPoint>     breakPointEdgeHandles;
	std::unordered_multimap<HalfedgeHandle, BreakPoint> breakPointHalfedgeHandles;
	std::unordered_multimap<FaceHandle, BreakPoint>     breakPointFaceHandles;
};

inline bool DebuggalbeMixin::shouldBreak(VertexHandle vh) const
{
	return breakPointVertexHandles.count(vh) != 0;
}
inline bool DebuggalbeMixin::shouldBreak(EdgeHandle eh) const
{
	return breakPointEdgeHandles.count(eh) != 0;
}
inline bool DebuggalbeMixin::shouldBreak(HalfedgeHandle heh) const
{
	return breakPointHalfedgeHandles.count(heh) != 0;
}
inline bool DebuggalbeMixin::shouldBreak(FaceHandle fh) const
{
	return breakPointFaceHandles.count(fh) != 0;
}

inline void DebuggalbeMixin::checkBreakPoints(VertexHandle vh, OpenMeshFunction fn) const
{
	if (disableCheck) return;
	auto its = breakPointVertexHandles.equal_range(vh);
	for (auto it = its.first; it != its.second; ++it) {
		if (it->second.condition(fn)) it->second.callback();
	}
}

inline void DebuggalbeMixin::checkBreakPoints(EdgeHandle eh, OpenMeshFunction fn) const
{
	if (disableCheck) return;
	auto its = breakPointEdgeHandles.equal_range(eh);
	for (auto it = its.first; it != its.second; ++it) {
		if (it->second.condition(fn)) it->second.callback();
	}
}

inline void DebuggalbeMixin::checkBreakPoints(HalfedgeHandle heh, OpenMeshFunction fn) const
{
	if (disableCheck) return;
	auto its = breakPointHalfedgeHandles.equal_range(heh);
	for (auto it = its.first; it != its.second; ++it) {
		if (it->second.condition(fn)) it->second.callback();
	}
}

inline void DebuggalbeMixin::checkBreakPoints(FaceHandle fh, OpenMeshFunction fn) const
{
	if (disableCheck) return;
	auto its = breakPointFaceHandles.equal_range(fh);
	for (auto it = its.first; it != its.second; ++it) {
		if (it->second.condition(fn)) it->second.callback();
	}
}

inline void DebuggalbeMixin::addBreakpoint(VertexHandle vh, const std::function<void()>& callback, Conditional condition)
{
	breakPointVertexHandles.insert(std::make_pair(vh, BreakPoint{ condition, callback }));
}
inline void DebuggalbeMixin::addBreakpoint(EdgeHandle eh, const std::function<void()>& callback, Conditional condition)
{
	breakPointEdgeHandles.insert(std::make_pair(eh, BreakPoint{ condition, callback }));
}
inline void DebuggalbeMixin::addBreakpoint(HalfedgeHandle heh, const std::function<void()>& callback, Conditional condition)
{
	breakPointHalfedgeHandles.insert(std::make_pair(heh, BreakPoint{ condition, callback }));
}
inline void DebuggalbeMixin::addBreakpoint(FaceHandle fh, const std::function<void()>& callback, Conditional condition)
{
	breakPointFaceHandles.insert(std::make_pair(fh, BreakPoint{ condition, callback }));
}

inline void DebuggalbeMixin::watch(VertexHandle vh)
{
	if (std::find(watchedVertexHandles.begin(), watchedVertexHandles.end(), vh) == watchedVertexHandles.end()) {
		watchedVertexHandles.push_back(vh);
	}
}
inline void DebuggalbeMixin::watch(EdgeHandle eh)
{
	if (std::find(watchedEdgeHandles.begin(), watchedEdgeHandles.end(), eh) == watchedEdgeHandles.end()) {
		watchedEdgeHandles.push_back(eh);
	}
}
inline void DebuggalbeMixin::watch(HalfedgeHandle heh)
{
	if (std::find(watchedHalfedgeHandles.begin(), watchedHalfedgeHandles.end(), heh) == watchedHalfedgeHandles.end()) {
		watchedHalfedgeHandles.push_back(heh);
	}
}
inline void DebuggalbeMixin::watch(FaceHandle fh)
{
	if (std::find(watchedFaceHandles.begin(), watchedFaceHandles.end(), fh) == watchedFaceHandles.end()) {
		watchedFaceHandles.push_back(fh);
	}
}

inline void DebuggalbeMixin::stopWatch(VertexHandle vh) {
	auto it = std::find(watchedVertexHandles.begin(), watchedVertexHandles.end(), vh);
	if (it == watchedVertexHandles.end()) throw std::runtime_error("Tried to end watch on vertex handle that is not beeing watched");
	watchedVertexHandles.erase(it);
}
inline void DebuggalbeMixin::stopWatch(EdgeHandle eh) {
	auto it = std::find(watchedEdgeHandles.begin(), watchedEdgeHandles.end(), eh);
	if (it == watchedEdgeHandles.end()) throw std::runtime_error("Tried to end watch on edge handle that is not beeing watched");
	watchedEdgeHandles.erase(it);
}
inline void DebuggalbeMixin::stopWatch(HalfedgeHandle heh) {
	auto it = std::find(watchedHalfedgeHandles.begin(), watchedHalfedgeHandles.end(), heh);
	if (it == watchedHalfedgeHandles.end()) throw std::runtime_error("Tried to end watch on halfedge handle that is not beeing watched");
	watchedHalfedgeHandles.erase(it);
}
inline void DebuggalbeMixin::stopWatch(FaceHandle fh) {
	auto it = std::find(watchedFaceHandles.begin(), watchedFaceHandles.end(), fh);
	if (it == watchedFaceHandles.end()) throw std::runtime_error("Tried to end watch on face handle that is not beeing watched");
	watchedFaceHandles.erase(it);
}
}
